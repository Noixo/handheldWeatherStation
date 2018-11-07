/*
 * sht20.c
 *
 * Created: 15/10/2018 1:32:33 PM
 *  Author: flato
 */ 
#include "sht20.h"
#include "i2c.h"
#include <util/delay.h>

void sht_init()
{
	i2c_start();
	i2c_device_id(SHT, WRITE);
	i2c_write(0x24);	//some setting thing
	i2c_stop();
}

void sht_register(char cmd)
{
	i2c_start();
	i2c_device_id(SHT, WRITE);
	i2c_write(SHTREGREAD);	//register address
	
	i2c_start();
	i2c_device_id(SHT, READ);
	
	//read reg
	(void) i2c_read(0);
	
	//write to reg
	i2c_start();
	i2c_device_id(SHT, WRITE);
	i2c_write(SHTREGWRITE);
	i2c_write(cmd);
	i2c_stop();
}

char sht_humidity()
{
	unsigned short raw_humidity;
	float humidity;

	i2c_start();
	i2c_device_id(SHT, WRITE);

	i2c_write(0xF5);	//humidity E5
	_delay_ms(100);

	i2c_start();
	i2c_device_id(SHT, READ);

	raw_humidity = i2c_read(0);
	raw_humidity <<= 8;
	raw_humidity |= i2c_read(1);	//ignore checksum
	
	i2c_stop();
	
	raw_humidity &= 0xFFFC;

	humidity = (125.0 * ((float)raw_humidity/65536.0));
	humidity -= 6.0;

	return humidity;
}

char sht_temp()
{
	unsigned short raw_temp;
	float temp;
	
	i2c_start();
	i2c_device_id(SHT, WRITE);
	i2c_write(0xF3);	//temp E3
	_delay_ms(100);
	
	i2c_start();
	i2c_device_id(SHT, READ);

	//read SHT20 data
	raw_temp = i2c_read(0);
	raw_temp <<= 8;
	raw_temp |= i2c_read(1);	//ignore checksum
	
	i2c_stop();
	
	raw_temp &= 0xFFFC;
	
	temp = (175.72 * ((float)raw_temp/65536.0));
	temp -= 46.85;
	
	return temp;
}