/*
 * sht20.c
 *
 * Created: 15/10/2018 1:32:33 PM
 *  Author: flato
 */ 
#include "sht20.h"
#include "i2c.h"
#include <avr/io.h>
#include "oled.h"

void sht_init()
{
	i2c_start();
	i2c_device_id(SHT, WRITE);
	i2c_write(0x24);	//some setting thing
	i2c_stop();
}

char sht_humidity()
{
	unsigned short humidity, raw_humidity;
	
	i2c_start();
	i2c_device_id(SHT, WRITE);
	i2c_write(0xE5);	//humidity
	
	i2c_start();
	i2c_device_id(SHT, READ);
	
	//set SCL to input
	DDRB &= ~(1 << PB2);	//set pin1 to input; turn off pin
	SCL_HIGH;				//enable pull up; turn on pin
	while(((PINB & (1 << PB2)) == 0));	//while SHT20 is low
	
	DDRB |= (1 << PB2);		//set pin2 to output

	//read SHT20 data
	//oled_write_int(i2c_read(0));
	//oled_write_char(' ');
	//oled_write_int(i2c_read(1));
	
	raw_humidity = i2c_read(0);
	raw_humidity <<= 8;
	raw_humidity |= i2c_read(1);	//ignore checksum
	
	raw_humidity &= 0xFFF0;	//and with resolution
	
	humidity = -6 + (125 * (raw_humidity/65536));
	
	return humidity;
}

char sht_temp()
{
	static unsigned short temp, raw_temp;
	
	i2c_start();
	i2c_device_id(SHT, WRITE);
	i2c_write(0xE3);	//temp
	
	i2c_start();
	i2c_device_id(SHT, READ);
	
	//set SCL to input
	DDRB &= ~(1 << PB2);	//set pin1 to input; turn off pin
	SCL_HIGH;				//enable pull up; turn on pin
	while(((PINB & (1 << PB2)) == 0));	//while SHT20 is low
		
	DDRB |= (1 << PB2);		//set pin2 to output

	//read SHT20 data
	raw_temp = i2c_read(0);
	raw_temp <<= 8;
	raw_temp |= i2c_read(1);	//ignore checksum
	
	temp = -46.85 + (175.72 * (raw_temp/65536));
	
	return temp;
}