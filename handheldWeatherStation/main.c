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

/************************************************************************/
/* TODO
                                                                     */
/************************************************************************/

#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#include "i2c.h"
#include "bmp280.h"
#include "oled.h"

//#include "font6x8.h"

void blink()
{
	//flash LED
	PORTB |= (1 << PB4);	//on
	_delay_ms(200);
	PORTB &= ~(1 << PB4);	//off
	_delay_ms(200);
	
	//flash LED
	PORTB |= (1 << PB4);	//on
	_delay_ms(200);
	PORTB &= ~(1 << PB4);	//off
}

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

int main(void)
{
	DDRB |= (1 << PB4); //set led to output
	
	init_i2c();
	
	blink();
	
	oled_write_string("\n");
	
	for (uint8_t i = 0; i < sizeof (ssd1306_init_sequence); i++) {
		oled_control(pgm_read_byte(&ssd1306_init_sequence[i]));
		/*
		i2c_start();
		i2c_device_id(OLED, WRITE);
		i2c_write(OLED_COMMAND);
		i2c_write(pgm_read_byte(&ssd1306_init_sequence[i]));
		*/
	}
	//i2c_stop();
	
	if(eeprom_read_byte(0) == 0xFF)	//if new ATTINY
	{
		bmpCalibration();	//run calibration to save data in EEPROM
		eeprom_write_byte(0, 0);
	}
	/*
	//oled_control(OLED_INVERT, 0);
	oled_control(OLED_CONTRAST);
	//_delay_ms(1000);
	oled_control(255);
	
	//oled_control(OLED_CONTRAST, OLED_COMMAND);
	//_delay_ms(1000);
	//oled_control(0, 1);
	//oled_control(0xA6, 1);
	oled_clear();
	
	oled_setpos(0,1);
	oled_write_string("Hello, world!");
	oled_setpos(0,3);
	//oled_clear();
	//_delay_ms(1000);
	oled_write_string("apple pie!");
	
	_delay_ms(1000);
	oled_clear();
	//oled_control(0xFF, OLED_DATA);
	*/
	oled_clear();
	//oled_write_string("Hello\n");
		
	//oled_write_string("THIS IS A TEST\n\n");
		
	//oled_write_string("THIS\n IS A LOOOOOOONNNNNGGGG    TEST\n");
		
	//oled_write_string("APPLE");
	
	//oled_write_int((bmp280GetTemp()/100));
	short bmpTemp;
	char INTbmpTemp;
	unsigned long bmpPressure;
	
	oled_setpos(0,0);
	//oled_write_int(eeprom_read_byte(1));
	//oled_setpos(0,1);
	//oled_write_int(eeprom_read_byte(2));
	//oled_setpos(0,1);
	//oled_write_int(dig_T2);
	//oled_setpos(0,2);
	//oled_write_int(dig_T3);
	bmpReset();
	
	//setting up sampling parameters
	bmpSet(0x64, CONFIG); //standby time = 250ms, IIR filter =
	bmpSet(0xFF, CTRL_MEAS); //x16 temperature oversampling, x16 pressure measurement, normal mode
	
	/*
	oled_write_int(eeprom_read_byte(1));
	oled_write_int(eeprom_read_byte(2));
	oled_write_char(' ');
	oled_write_int(eeprom_read_byte(3));
	oled_write_int(eeprom_read_byte(4));
	oled_write_char(' ');
	oled_write_int(eeprom_read_byte(5));
	oled_write_int(eeprom_read_byte(6));
	oled_write_char(' ');
	oled_write_int(eeprom_read_byte(7));
	oled_write_int(eeprom_read_byte(8));
	oled_write_char(' ');
	oled_write_int(eeprom_read_byte(9));
	oled_write_int(eeprom_read_byte(10));
	oled_write_char(' ');
	oled_write_int(eeprom_read_byte(11));
	oled_write_int(eeprom_read_byte(12));
	oled_write_char(' ');
	oled_write_int(eeprom_read_byte(13));
	oled_write_int(eeprom_read_byte(14));
	oled_write_char(' ');
	oled_write_int(eeprom_read_byte(15));
	oled_write_int(eeprom_read_byte(16));
	oled_write_char(' ');
	oled_write_int(eeprom_read_byte(17));
	oled_write_int(eeprom_read_byte(18));
	oled_write_char(' ');
	oled_write_int(eeprom_read_byte(19));
	oled_write_int(eeprom_read_byte(20));
	oled_write_char(' ');
	oled_write_int(eeprom_read_byte(21));
	oled_write_int(eeprom_read_byte(22));
	oled_write_char(' ');
	oled_write_int(eeprom_read_byte(23));
	oled_write_int(eeprom_read_byte(24));
	oled_write_char(' ');
	*/
    while (1)
    {
		//oled_write_int((sht20_temp() / 100);
		//oled_write_int((sht20_temp() % 100));
		/*
		bmpPressure = bmp280GetPressure();
		bmpTemp = bmp280GetTemp();
		INTbmpTemp = bmpTemp / 100;
		
		oled_setpos(0, 0);
		oled_write_string("Temperature: ");
		oled_write_int(INTbmpTemp);
		
		oled_write_char('.');
		oled_write_int(bmpTemp % 100);
		
		oled_setpos(0,2);
		oled_write_string("Pressure: ");
		oled_write_int(bmpPressure / 1000);
		oled_write_int((bmpPressure % 1000) / 100);
		oled_write_char('.');
		oled_write_int(bmpPressure % 100);	//add <10 check thing
		*/
		/*
		bmpTemp = bmp280GetTemp();
		INTbmpTemp = bmpTemp / 100;
		bmpPressure = bmp280GetPressure();
		
		oled_write_int((INTbmpTemp));
		oled_write_char('.');
		oled_write_int((bmpTemp % 100));
		oled_setpos(0,2);
		
		oled_write_int(bmpPressure / 1000);
		oled_write_int((bmpPressure % 1000) / 100);
		oled_write_char('.');
		oled_write_int(bmpPressure % 100);	//add <10 check thing
		*/
		//_delay_ms(10);
		//oled_write_char(' ');
		/*
		oled_fill();
		_delay_ms(1000);
		oled_clear();
		*/
		blink();
		//oled_control(0xFF, OLED_DATA);
		//_delay_ms(100);	
    }
	return 0;
}

