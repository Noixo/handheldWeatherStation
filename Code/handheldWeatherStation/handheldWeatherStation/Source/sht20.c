/*
 * sht20.c
 *
 * Created: 15/10/2018 1:32:33 PM
 *  Author: flato
 */ 
#include "sht20.h"
#include "i2c.h"
#include <util/delay.h>
#include "oled.h"

void sht_init()
{
	i2c_start();
	i2c_device_id(SHT, WRITE);
	i2c_write(0x24);	//some setting thing
	i2c_stop();
}

//WARNING: no reserved bit protection
void sht_register(char cmd)
{
	//char tmp;
	
	i2c_start();
	i2c_device_id(SHT, WRITE);
	i2c_write(SHTREGREAD);	//register address	
	i2c_stop();
	
	i2c_start();
	i2c_device_id(SHT, READ);
	
	//read reg
	i2c_read(1);	//read temp settings tmp = 
	i2c_stop();
	
	
	//oled_setpos(0,7);
	//oled_write_int(tmp);
	
	//char bitmask = tmp &= 0x1C;	//get reserved bits
	
	
	//cmd	&= (tmp & 0x1C);//overwrite reserved bits on user bits
	//write to reg
	i2c_start();
	i2c_device_id(SHT, WRITE);
	i2c_write(SHTREGWRITE);
	i2c_write(cmd | 0x18);
	i2c_stop();
}

float sht_humidity()
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

float sht_temp()
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