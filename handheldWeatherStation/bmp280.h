#ifndef _BMP280_H_
#define _BMP280_H_

//define the i2c address for the BMP280
#define bmp280 0x77

//config settings
#define CTRL_MEAS 0xF4
#define CONFIG 0xF5

//global variable to store temperature in (used for pressure)
extern long t_fine;

//calibration data

#define dig_T1 27314
#define dig_T2 26369
#define dig_T3 -1000

#define dig_P1 38955
#define dig_P2 -10633
#define dig_P3 3024
#define dig_P4 2484
#define dig_P5 118
#define dig_P6 -7
#define dig_P7 15500
#define dig_P8 -14600
#define dig_P9 6000


/*
#define dig_T1 eeprom_read_word(1)//27314
#define dig_T2 eeprom_read_word(3)//26369
#define dig_T3 eeprom_read_word(5)//-1000

#define dig_P1 eeprom_read_word(7)//38955
#define dig_P2 eeprom_read_word(9)//-10633
#define dig_P3 eeprom_read_word(11)//3024
#define dig_P4 eeprom_read_word(13)//2484
#define dig_P5 eeprom_read_word(15)//118
#define dig_P6 eeprom_read_word(17)//-7
#define dig_P7 eeprom_read_word(19)//15500
#define dig_P8 eeprom_read_word(21)//-14600
#define dig_P9 eeprom_read_word(23)//6000
*/
extern void bmpReset();	//const
extern void bmpCalibration();	//const
extern unsigned long bmp280GetPressure();
extern long bmp280GetTemp();
extern void bmpSet(unsigned char settings, unsigned char reg);

#endif