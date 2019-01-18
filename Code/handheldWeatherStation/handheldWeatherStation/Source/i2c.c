#include <avr/io.h>
#include <util/delay.h>

//#include <stdio.h>

#include "i2c.h"

void init_i2c()
{
	DDRB |= (1 << PB0);	// Set SDA as output
	DDRB |= (1 << PB2);	// Set SCL as output
	
	SDA_HIGH;
	SCL_HIGH;
}

void i2c_clock()
{
	SCL_HIGH;
	_delay_us(TIME);
	SCL_LOW;
}

void i2c_start()
{
	//Clock stays high and SDA pulled low
	SDA_HIGH;
	SCL_HIGH;

	_delay_us(TIME);
	SDA_LOW;
	_delay_us(TIME);
	SCL_LOW;
}

void i2c_stop()
{
	//clock goes low before data
	SCL_LOW;
	SDA_LOW;
	_delay_us(TIME);

	SCL_HIGH;
	_delay_us(TIME);
	SDA_HIGH;
}

char i2c_device_id(volatile char id, volatile char rw)
{
	char i, ACK;
	//id = 0x77;//0x76;//0x3C;//78;

	//check if valid i2c address
	if(id < 8 || id > 120)
		return 2;

	//Starting at 7 to avoid last bit
	for(i = 7; i > 0; i--)
	{
		id <<= 1;
		if((id & 0x80))
			SDA_HIGH;
		else
			SDA_LOW;
		//SDA = id & 0x80;	//Sends a 1 or 0 to SDA

		//Pulses the clock
		i2c_clock();
	}
	//8th bit. pulses the read/write bit
	//rw low for write and high for read

	if(rw == 1)
		SDA_HIGH;
	else
		SDA_LOW;
	i2c_clock();
	//SDA = rw;
	
	//9th bit
	//set SDA to input
	DDRB &= ~(1 << PB0);	//set pin0 to input; turn off pin
	SDA_HIGH;//PORTB |= (1 << PB0);	//enable pull up; turn on pin
	ACK = (PINB & (1 << PB0));	//read pin

	i2c_clock();
	
	DDRB |= (1 << PB0);	//set pin0 to output

	return ACK;
}

unsigned char i2c_read(char last_byte)
{
	char i = 0, byte = 0;
	SDA_HIGH;
	
	DDRB &= ~(1 << PB0);	//set pin0 to input; turn off pin
	SDA_HIGH;				//enable pull up; turn on pin
	
	for(i = 8; i > 0; i--)
	{	
		SCL_HIGH;
		//bitshift byte by 1
		byte <<= 1;

		//read pin
		
		//OR byte bit with SDA
		byte |= (PINB & (1<<PB0));

		i2c_clock();
	}
	DDRB |= (1 << PB0);	//set pin0 to output
	
	//9th bit master acknowledges data transfer or indicates last byte
	if(last_byte == 1)
		SDA_HIGH;
	else
		SDA_LOW;
	//SDA = last_byte;
	
	i2c_clock();
	
	return byte;
}

void i2c_write(unsigned char byte)
{
	char i;
	for(i = 8; i > 0; i--)
	{
		//bit shifts data by i and ANDs it to convert it to boolean
		if((byte & 0x80))
			SDA_HIGH;
		else
			SDA_LOW;
		//SDA = byte & 0x80;	//Sends a 1 or 0 to SDA
		
		//pulse the clock
		i2c_clock();
		
		byte <<= 1;
	}
	
	//set to high to detect ack bit
	SDA_HIGH;
	
	i2c_clock();

}

//scans the bus to find all i2c devices
void i2c_read_id()
{
	char i, ACK;
	
	//search through all valid i2c addresses
	for(i = 8; i < 120; i++)	//7 bit addressing
	{
		i2c_start();
		
		ACK = i2c_device_id(i, 0);	//gets the ACK bit after sending i2c device address
		
		i2c_stop();
		//if ACK = 0 then device was found
		if(ACK == 0)
			{
				//oled_write_int(i);
				//oled_write_char(' ');
				PORTB |= (1 << PB4);
				_delay_ms(200);
				PORTB &= ~(1 << PB4);
				_delay_ms(200);
			}
	}
}