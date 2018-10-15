/*
 * sht20.h
 *
 * Created: 15/10/2018 1:32:50 PM
 *  Author: flato
 */ 

#ifndef SHT20_H_
#define SHT20_H_

#define SHT20 0x40

#define SHT20T 0xE3;
#define SHT20H 0xE5;

char sht20_humidty();
char sht20_temp();

#endif