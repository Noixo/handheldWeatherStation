/*
 * sht20.c
 *
 * Created: 15/10/2018 1:32:33 PM
 *  Author: flato
 */ 
#include "sht20.h"
#include "i2c.h"
#include <math.h>
#include <avr/io.h>

char sht20_humidity()
{
	unsigned char humidity, raw_humidity;
	
	humidity = -6 + 125 * (raw_humidity/(pow(2, 16)));
	
	return humidity;
}

char sht20_temp()
{
	static unsigned short temp, raw_temp;
	
	i2c_start();
	i2c_device_id(SHT20, WRITE);
	i2c_write(SHT20T);
	
	i2c_start();
	i2c_device_id(SHT20, READ);
	
	//set SCL to input
	DDRB &= ~(1 << PB2);	//set pin1 to input; turn off pin
	SCL_HIGH;				//enable pull up; turn on pin
	while(((PINB & (1 << PB0)) == 0));	//while SHT20 is low
		
	DDRB |= (1 << PB0);		//set pin2 to output

	//read SHT20 data
	raw_temp = i2c_read(0);
	raw_temp <<= 8;
	raw_temp |= i2c_read(1);	//ignore checksum
	
	temp = -46.85 + 175.72 * (raw_temp/(pow(2, 16)));
	
	return temp;
}