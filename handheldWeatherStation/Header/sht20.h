/*
 * sht20.h
 *
 * Created: 15/10/2018 1:32:50 PM
 *  Author: flato
 */ 

#ifndef _SHT_H_
#define _SHT_H_

#define SHT 0x40

#define SHTTEMP 0xE3
#define SHTHUM 0xE5
#define SHTREGREAD 0xE7
#define SHTREGWRITE 0xE6

void sht_init();
void sht_register(char cmd);
float sht_humidity();
float sht_temp();

#endif