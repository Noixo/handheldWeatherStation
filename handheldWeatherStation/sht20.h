/*
 * sht20.h
 *
 * Created: 15/10/2018 1:32:50 PM
 *  Author: flato
 */ 

#ifndef SHT_H_
#define SHT_H_

#define SHT 0x40

#define SHTTEMP 0xE3;
#define SHTHUM 0xE5;

void sht_init();
char sht_humidity();
char sht_temp();

#endif