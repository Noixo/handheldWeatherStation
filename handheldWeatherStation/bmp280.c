#include "bmp280.h"
#include "i2c.h"
#include <avr/eeprom.h>
#include <util/delay.h>

//global variable to store value needed for pressure measurement
long t_fine = 0;

void bmpCalibration()
{
	unsigned char i;
	uint16_t* j = (uint16_t*)0;
	unsigned short arr[12];
	//short temp;	//stores 2 byte calibration data
	
	i2c_start();
	
	i2c_device_id(bmp280, 0);
	
	i2c_write(0x88);	//start of calibration data
	
	//quick start
	i2c_start();
	i2c_device_id(bmp280, 1);
	
	for(i = 0; i < 12; i++)	//write calibration values to EEPROM or return pointer?
	{
		arr[i] = i2c_read(0);
		arr[i] <<= 8;
		arr[i] |= i2c_read(0);
		arr[i] = (arr[i] >> 8) | (arr[i] << 8);
	}
	i2c_read(1);
	
	//arr[sizeof(arr)] = i2c_read(1);
	
	i2c_stop();
	
	for(i = 0, j = (uint16_t *)1 ; i < 12; i++)
	{
		eeprom_write_word((uint16_t *)j, arr[i]);
		j++;
		while(!eeprom_is_ready());	//wait for eeprom to write data
	}
}

void bmpReset()
{
	i2c_start();
	(void) i2c_device_id(bmp280, 0);
	i2c_write(0xE0);
	
	i2c_start();
	(void) i2c_device_id(bmp280, 0);
	i2c_write(0xB6);
	i2c_stop();
}

void bmpSet(unsigned char settings, unsigned char reg)
{
	i2c_start();
	i2c_device_id(bmp280, 0);
	i2c_write(reg);

	i2c_write(settings);
	i2c_stop();
}

//must read temp before reading pressure
//temp is stored as 3 bytes, (0xFA-0xFC)
long bmp280GetTemp()
{
	//Keil long = 4 bytes, int = 2
	long adc_T, var1, var2;
	short T;
	
	//take out of sleep, into forced mode
	bmpSet(0x25, CTRL_MEAS); //x1 pressure, x1 temperature, power mode
	_delay_ms(10);
	
	
	i2c_start();
	i2c_device_id(bmp280, 0);

	i2c_write(0xFA);

	i2c_start();
	
	i2c_device_id(bmp280, 1);
	
	//shove temperature data into a 4 byte variable
	adc_T = i2c_read(0);
	adc_T <<= 8;
	adc_T |= i2c_read(0);
	adc_T <<= 8;
	adc_T |= i2c_read(1);
	i2c_stop();

	//conversion formula takes 20 bits
	adc_T >>= 4;

	//32bit-fixed point formula from BMP280 documentation -pg45-46 (8.2)

	var1 = ((((adc_T>>3) - ((long)dig_T1<<1))) * ((long)dig_T2)) >> 11;
	var2  = (((((adc_T>>4) - ((long)dig_T1)) * ((adc_T>>4)
		- ((long)dig_T1))) >> 12) * ((long)dig_T3)) >> 14;

	t_fine = var1+var2;
	T = (t_fine*5+128) >> 8;

	return (T);
}

unsigned long bmp280GetPressure()
{
	long var1, var2, adc_P;
	unsigned long p;
	
	//take out of sleep, into forced mode
	bmpSet(0x25, CTRL_MEAS); //x1 pressure, x1 temperature, IIR off
	_delay_ms(10);
	
	
	//t_fine = t_fine;
	//begin multi-byte data transfer
	i2c_start();
	(void) i2c_device_id(bmp280, 0);
	//start at 0xF7-0xF9
	//point to pressure data location
	i2c_write(0xF7);

	//repeated start
	i2c_start();
	(void) i2c_device_id(bmp280, 1);

	adc_P = i2c_read(0);
	adc_P <<= 8;
	adc_P |= i2c_read(0);
	adc_P <<= 8;
	adc_P |= i2c_read(1);

	i2c_stop();

	//only use 20 bits
	adc_P >>= 4;

	//pressure conversion formula from BMP280 documentation
	//32bit-fixed point conversion -pg 46

	var1 = (((long)t_fine) >> 1) - (long)64000;
	var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((long)dig_P6);
	
	var2 = var2 + ((var1 * ((long)dig_P5)) << 1);
	var2 = (var2 >> 2) + (((long)dig_P4) << 16);

	var1 = (((dig_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3)
	+ ((((long)dig_P2) * var1) >> 1)) >> 18;

	var1 = ((((32768 + var1)) * ((long) dig_P1)) >> 15);

	if(var1 == 0)
	{
		return 0;
	}
	p = (((unsigned long) (((long)1048576) - adc_P) - (var2 >> 12))) * 3125;

	if(p < 0x80000000)
	{
		p = (p << 1) / ((unsigned long)var1);
	}
	else
	{
		p = (p / (unsigned long)var1) * 2;
	}

	var1 = (((long) dig_P9) * ((long) (((p >> 3) * (p >> 3)) >> 13))) >> 12;
	var2 = (((long)(p >> 2)) * ((long) dig_P8)) >> 13;

	p = (unsigned long)((long)p + ((var1 + var2 + dig_P7) >> 4));
	
	return p;
}