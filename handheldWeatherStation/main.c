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

/*
current usage: (not including boost)

Display full: 4.6mA-5mA approx
Display dim: 3mA
Display sleep: 1.3mA - 1.5mA approx
Micro sleep:
*/


#define F_CPU 1000000UL

#define VERSION "1.01"
#define DATE "19.10.18"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

#include <string.h>
#include <stdlib.h>

#include "i2c.h"
#include "bmp280.h"
#include "oled.h"
#include "sht20.h"
#include "oledCustomChar.h"

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

void splash_screen()
{
	//bitmap image
	
	oled_setpos((OLEDMID - 16), 6);
	oled_write_string("Matthew Whiteley");
	
	oled_setpos((OLEDMID - strlen(VERSION)), 7);
	oled_write_string(VERSION);
	
	oled_setpos((OLEDRIGHT - strlen(DATE)), 7);
	oled_write_string(DATE);
	
	
}

void display()
{
	//short bmpTemp;
	//char INTbmpTemp;
	unsigned long bmpPressure;
	
	bmp280GetTemp();
	//bmpTemp = bmp280GetTemp();
	//INTbmpTemp = bmpTemp / 100;
	bmpPressure = bmp280GetPressure();
	
	oled_setpos(0,1);
	
	oled_write_string("Temperature: ");
	oled_write_int((char)sht_temp());
	oled_write_string("*C");
	
	oled_setpos(0,3);
	
	oled_write_string("Humidity: ");
	oled_write_int((char)sht_humidity());
	oled_write_string("%RH");
	
	/*
	oled_write_int((INTbmpTemp));
	oled_write_char('.');
	oled_write_int((bmpTemp % 100));
	*/
			
	oled_setpos(0,5);
	oled_write_string("Pressure: ");
	oled_write_int(bmpPressure / 1000);
	oled_write_int((bmpPressure % 1000) / 100);
	oled_write_char('.');
	if((bmpPressure % 100) < 10)
		oled_write_char('0');
	oled_write_int(bmpPressure % 100);	//add <10 check thing
	oled_write_string("HPa");
	
	//oled_setpos(OLEDX - 4, 7);
	//oled_write_string(VERSION);
}

// 0 = empty
// 4 = full
void battery()
{
	unsigned char convert;
	
	//get ADC
	ADCSRA |= (1 << ADSC);
	while(ADCSRA & (1 << ADSC));	//wait till done
	
	oled_setpos(OLEDX - 6,0);

	//2V-3.3V
	if(ADCH < 255 && ADCH > 213)	//full
		convert = 4;
		
	if(ADCH < 213 && ADCH > 192)	//75%
		convert = 3;
		
	if(ADCH < 192 && ADCH > 171)	//50%
		convert = 2;
	
	if(ADCH < 171 && ADCH > 150)	//25%
		convert = 1;
	
	if(ADCH < 150)	//ded @2V
		convert = 0;

	for(char i = 0; i < 6; i++)
	{
		oled_data(pgm_read_byte(&oled_customChar[convert * 6 + i]));
	}
}

void adc_init()
{
	//VCC ref, PB4 input, ADLAR = 1
	ADMUX = (1 << MUX1);
	ADMUX |= (1 << ADLAR);
			
	ADCSRA = (1 << ADEN);
	ADCSRA |= (1 << ADPS2);
	ADCSRA |= (1 << ADPS1);
	
}

ISR(WDT_vect)
{
	wdt_disable();
}

void sleep()
{
	
	ADCSRA = (0 << ADEN);	//turn off?
	
	
	MCUSR &= ~(1<<WDRF);
	
	WDTCR |= 0x18;	//enable watchdog

	WDTCR = 0x46;	//turn on interrupt
	
	sei();	//enble interrupt
	
	wdt_reset();
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_mode();
	
	ADCSRA = (1 << ADEN);	//turn on ADC
}

int main(void)
{
	_delay_ms(100);	//delay to let voltage stablise
	
	DDRB |= (1 << PB4); //set led to output
	PORTB = 0xFF;	//turn on all pullups
	
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
	
	//setting up sampling parameters
	bmpSet(0x64, CONFIG); //standby time = 250ms, IIR filter =
	bmpSet(0xFF, CTRL_MEAS); //x16 temperature oversampling, x16 pressure measurement, normal mode


	//change contrast to 0 (lowest)
	oled_control(0x81);
	oled_control(0x00);
	
	//clear oled of random data
	oled_clear();
	
	splash_screen();
	_delay_ms(1000);
	oled_clear();
	
    while(1)
    {
		//check battery level
		battery();

		//display data
		display();
		
		//sleep for 1 sec
		sleep();
		//_delay_ms(1000);
    }
	return 0;
}