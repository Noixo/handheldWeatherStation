/*
 * handheldWeatherStation.c
 *
 * Created: 19/02/2018 00:16:46
 * Author : flato
 */ 

/*
PB0: MOSI
PB1: MISO
PB2: SCK
PB3: button
PB4: led/batt
PB5: RST
*/

/*	TODO
	- reorganise main.c
	- fix < 10 glich (both *C and .09)
	- make sure < 0 temp works
	- make BMP280 forced 
	- turn off sht20 heater?
*/

/*
current usage: (not including boost)

Display full: 4.6mA-5mA approx
Display dim: 3mA
Display sleep: 1.3mA - 1.5mA approx
Micro sleep + display off + no data: 0.0075mA
*/

#define DATE "19.10.18"

#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#include <stdio.h>

#include <string.h>

#include "i2c.h"
#include "bmp280.h"
#include "oled.h"
#include "sht20.h"
#include "powerSave.h"
#include "display.h"

const uint8_t ssd1306_init_sequence [] PROGMEM = {	// Initialization Sequence
	0xAE,			// Display OFF (sleep mode)
	0x20, 0b00,		// Set Memory Addressing Mode
	// 00=Horizontal Addressing Mode; 01=Vertical Addressing Mode;
	// 10=Page Addressing Mode (RESET); 11=Invalid
	0xB0,			// Set Page Start Address for Page Addressing Mode, 0-7
	0xC8,			// Set COM Output Scan Direction
	0x00,			// ---set low column address
	0x10,			// ---set high column address
	0x40,			// --set start line address
	0x81, 0x3F,		// Set contrast control register
	0xA1,			// Set Segment Re-map. A0=address mapped; A1=address 127 mapped.
	0xA6,			// Set display mode. A6=Normal; A7=Inverse
	0xA8, 0x3F,		// Set multiplex ratio(1 to 64)
	0xA4,			// Output RAM to Display
	// 0xA4=Output follows RAM content; 0xA5,Output ignores RAM content
	0xD3, 0x00,		// Set display offset. 00 = no offset
	0xD5,			// --set display clock divide ratio/oscillator frequency
	0xF0,			// --set divide ratio
	0xD9, 0x22,		// Set pre-charge period
	0xDA, 0x12,		// Set com pins hardware configuration
	0xDB,			// --set vcomh
	0x20,			// 0x20,0.77xVcc
	0x8D, 0x14,		// Set DC-DC enable
	0xAF			// Display ON in normal mode
};

void butt_press()
{
	if((PINB & (1 << PB3)))	//read pin state. If LOW then button pressed
	{
		oled_control(0xAE);	//turn off display
	}
	//turn on display
	else
	{
		oled_control(0xAF);	
		//check battery level
		battery();

		//display data
		display();		
	}
}

int main(void)
{
	PORTB = (0xFF);	//turn on all pullups
	_delay_ms(400);	//delay to let voltage stablise
	
	adc_init();
	init_i2c();
	
	//start up OLED
	for (uint8_t i = 0; i < sizeof (ssd1306_init_sequence); i++) {
		oled_control(pgm_read_byte(&ssd1306_init_sequence[i]));
	}
	
	if(eeprom_read_byte((uint8_t*)0x00) == 0xFF)	//if new ATTINY
	{
		bmpCalibration();	//run calibration to save data in EEPROM
		eeprom_write_byte(0, 0);
	}

	bmpReset();
	
	//change contrast to brightest
	oled_control(0x81);
	oled_control(0xFF);
	
	//clear oled of random data
	oled_clear();
	
    while(1)
    {
		butt_press();

		//sleep for 1 sec
		sleep();
    }
	return 0;
}