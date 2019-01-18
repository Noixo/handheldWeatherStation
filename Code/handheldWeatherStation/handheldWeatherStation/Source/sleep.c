/*
 * sleep.c
 *
 * Created: 22/10/2018 12:49:40 AM
 *  Author: flato
 */ 
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>

volatile char portState;
/*
ISR(TIMER1_OVF_vect)
{
	sleep_disable();
}

void sleepInit()
{
	//save port settings
	portState = DDRB;
	//set all pins to input
	DDRB = 0x00;
}

void sleep()
{
	set_sleep_mode(SLEEP_MODE_IDLE);
	
	//configure interrupt
	
	TCNT1 = 3998;	//1 sec 1MHz?
	
	//TCCR1A = 0x00;
	//TCCR
	
	sleep_enable();
	
//	sleep_disable();
	
	DDRB = portState;
}
*/