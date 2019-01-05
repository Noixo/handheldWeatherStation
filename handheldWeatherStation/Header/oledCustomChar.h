/*
 * oledCustomChar.h
 *
 * Created: 16/10/2018 8:24:20 PM
 *  Author: flato
 */ 

#ifndef _OLEDCUSTOMCHAR_H_
#define _OLEDCUSTOMCHAR_H_
// ----------------------------------------------------------------------------

#include <avr/pgmspace.h>

// ----------------------------------------------------------------------------

const unsigned char oled_customChar[] PROGMEM = {
	0x0D, 0x1B, 0x11, 0x11, 0x11, 0x1F, //empty battery
	0x0D, 0x1B, 0x11, 0x11, 0x1F, 0x1F, //25% battery
	0x0D, 0x1B, 0x11, 0x1F, 0x1F, 0x1F,	//50%
	0x0D, 0x1B, 0x1F, 0x1F, 0x1F, 0x1F, //75% battery
	0x0D, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, //full battery
};

#endif