/*
 * display.c
 *
 * Created: 14/01/2019 10:47:14 PM
 *  Author: Matt
 */ 

#include <avr/io.h>
#include <stdlib.h>
#include "display.h"
#include "bmp280.h"
#include "sht20.h"
#include "oled.h"
#include "oledCustomChar.h"

#define VERSION "1.1"

/*
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
*/
void adc_init()
{
	//VCC ref, PB4 input, ADLAR = 1
	ADMUX = (1 << MUX1);
	ADMUX |= (1 << ADLAR);
	
	ADCSRA = (1 << ADEN);
	ADCSRA |= (1 << ADPS2);
	ADCSRA |= (1 << ADPS1);
	
}

void print_float(float value)
{
	char arr[5];
	
	
	dtostrf(value, 4, 2, arr);
	//arr[5] = '/0';	//only 2 decimal points
	
	oled_write_string(arr);
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
	
	//oled_clear_ln(1);
	oled_setpos(0,1);
	
	oled_write_string("Temperature: ");
	if((char)sht_temp() < 0)
	{
		oled_write_char('-');
		print_float(!sht_temp());	//convert value to "negative"
	}
	else
	{
		print_float(sht_temp());

	}
	if(sht_temp() < 10)	//remove extra *C
		oled_write_char(' ');
		
	oled_write_string("*C");
	
	
	oled_setpos(0,3);
	
	oled_write_string("Humidity: ");
	print_float(sht_humidity());

	oled_write_string("%RH");
	
	oled_setpos(0,5);
	oled_write_string("Pressure: ");
	oled_write_int(bmpPressure / 1000);
	oled_write_int((bmpPressure % 1000) / 100);
	oled_write_char('.');
	if((bmpPressure % 100) < 10)
		oled_write_char('0');
	oled_write_int(bmpPressure % 100);	//add <10 check thing
	oled_write_string("HPa");
	
	oled_setpos(0, 7);
	oled_write_string(VERSION);
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