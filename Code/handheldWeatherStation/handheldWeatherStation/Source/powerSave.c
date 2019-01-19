/*
 * powerSave.c
 *
 * Created: 14/01/2019 10:41:47 PM
 *  Author: Matt
 */ 

#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include "powerSave.h"

ISR(WDT_vect)
{
	wdt_disable();
}

void onWake()
{
	//wake
	ADCSRA = (1 << ADEN);	//turn on ADC
	
	DDRB |= (1 << PB0);	// Set SDA as output
	DDRB |= (1 << PB2);	// Set SCL as output
}

void sleep()
{
	//set output pins to input
	DDRB &= ~(1 << PB0);	//set sda to input; turn off pin
	DDRB &= ~(1 << PB2);	//set sck to input; turn off pin
	
	ADCSRA = (0 << ADEN);	//turn off?
	
	
	MCUSR &= ~(1<<WDRF);
	
	WDTCR |= 0x18;	//enable watchdog

	WDTCR = 0x46;	//turn on interrupt
	
	sei();	//enble interrupt
	
	wdt_reset();
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_mode();
	
	//in sleep?
}
