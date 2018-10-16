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
#define dig_T1 (uint16_t)eeprom_read_word((uint16_t*)1)
#define dig_T2 (int16_t)eeprom_read_word((uint16_t*)3)
#define dig_T3 (int16_t)eeprom_read_word((uint16_t*)5)

#define dig_P1 (uint16_t)eeprom_read_word((uint16_t*)7)
#define dig_P2 (int16_t)eeprom_read_word((uint16_t*)9)
#define dig_P3 (int16_t)eeprom_read_word((uint16_t*)11)
#define dig_P4 (int16_t)eeprom_read_word((uint16_t*)13)
#define dig_P5 (int16_t)eeprom_read_word((uint16_t*)15)
#define dig_P6 (int16_t)eeprom_read_word((uint16_t*)17)
#define dig_P7 (int16_t)eeprom_read_word((uint16_t*)19)
#define dig_P8 (int16_t)eeprom_read_word((uint16_t*)21)
#define dig_P9 (int16_t)eeprom_read_word((uint16_t*)23)


extern void bmpReset();	//const
extern void bmpCalibration();	//const
extern unsigned long bmp280GetPressure();
extern long bmp280GetTemp();
extern void bmpSet(unsigned char settings, unsigned char reg);

#endif