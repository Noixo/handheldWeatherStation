/*
 * handheldWeatherStation.c
 *
 * Created: 19/02/2018 00:16:46
 * Author : Matt
 */ 

/*
PB0: MOSI
PB1: MISO
PB2: SCK
PB3: button
PB4: led/batt
PB5: RST
*/

/*
current usage: (not including boost)

Display full: 4.6mA-5mA approx
Display dim: 3mA
Display sleep: 1.3mA - 1.5mA approx
Micro sleep + display off + no data: 0.0075mA
*/

#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/eeprom.h>

#include "i2c.h"
#include "bmp280.h"
#include "oled.h"
#include "sht20.h"
#include "powerSave.h"
#include "display.h"

char microWake = 0xFF;

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
	if((PINB & (1 << PB3)) || microWake)	//read pin state. If HIGH then button NOT pressed & stop sending screen off cmd
	{
		oled_control(0xAE);	//turn off display
		microWake = 0;
	}
	//turn on display
	else
	{
		onWake();	//wake up peripherals 
		
		oled_control(0xAF);	//Turn on display
		
		//check battery level
		battery();

		//generate and display data
		display();
		
		//send off cmd if button not pushed (dumb ik)
		if(PINB & (1 << PB3))
			microWake = 0xFF;
	}
}

int main(void)
{
	PORTB = (0xFF);	//turn on all pullups
	_delay_ms(400);	//delay to let voltage stabilise
	
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

		//deep sleep for 1 sec
		sleep();
    }
	return 0;
}