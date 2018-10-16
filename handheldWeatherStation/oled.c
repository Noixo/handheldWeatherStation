/*
 * oled.c
 *
 * Created: 19/02/2018 00:19:02
 *  Author: flato
 */ 

#include <avr/pgmspace.h>

#include "oled.h"
#include "font6x8.h"
#include "i2c.h"
#include "subroutine.h"

char pos_x;
char pos_y;
/*
void oled_checkposx()
{
	if(pos_x > OLEDX)
	{
		pos_x = 0;
		oled_setpos(0, ++pos_y);
	}
	else
		pos_x++;
}
*/
/*sets cursor position.
 ARGS: x = 0-127, y = 0-7
*/
void oled_setpos(char x, char y)
{
	//add data verification
	oled_control(OLED_COLUMNADDR);
	oled_control(x);
	oled_control(127);
	
	oled_control(OLED_PAGEADDR);
	oled_control(y);
	oled_control(7);
}

/*fills the whole screen with white/black (depends on inverse or not)
*/
void oled_fill()
{
	unsigned char page, x;
	
	oled_setpos(0, 0);
	for(page = 0; page <= OLEDY; page++)
	{
		for(x = 0; x <= OLEDX; x++)
			oled_data(0xFF);	//white
	}
	oled_setpos(0, 0);
}

/*clears the display with blank
*/
void oled_clear()
{
	unsigned char page, x;
	
	oled_setpos(0, 0);
	
	for(page = 0; page <= OLEDY; page++)
	{
		for(x = 0; x <= OLEDX; x++)
			oled_data(0x00);
	}
	oled_setpos(0, 0);
}

/*send commands to oled
*/
void oled_control(char command)
{
	i2c_start();
	i2c_device_id(OLED, WRITE);
	i2c_write(OLED_COMMAND);		//control byte
	i2c_write(command);	
	i2c_stop();
}

/*send data to oled
*/
void oled_data(char data)
{
	//oled_checkposx();	//check if data in range
	
	i2c_start();
	i2c_device_id(OLED, WRITE);
	i2c_write(OLED_DATA);		//DATA byte
	i2c_write(data);
	i2c_stop();
}

void oled_write_char(char ch)
{
	//oled_checkposx();
	
	if(ch == '\n')	//go to newline
	{
		if(pos_y < 8)
			oled_setpos(0, ++pos_y);
		else
		{
			pos_y = 0;
			oled_setpos(0, ++pos_y);
		}
		
		return;
	}
	
	//convert ASCII to map address
	ch -= 32;
	i2c_start();
	i2c_device_id(OLED, WRITE);
	i2c_write(OLED_DATA);

	for (char i= 0; i < 6; i++)
	{
		i2c_write(pgm_read_byte(&oled_font6x8[ch * 6 + i]));	//*6 to select char, i increments through 
	}
	i2c_stop();
}

void oled_write_string(char *str)
{
	while(*str)
	{
		oled_write_char(*str++);
	}
	//oled_write_char(pos_x);
}

void oled_write_int(unsigned char value)
{

	char *arr;
	
	arr = convert(value);
	
	while(*arr != '\0')
		oled_write_char(*arr++);
}