/*
 * oled.h
 *
 * Created: 19/02/2018 00:20:52
 *  Author: flato
 */ 


#ifndef OLED_H_
#define OLED_H_

//extern char pos_x;
//extern char pos_y;

#define OLEDX 127
#define OLEDY 7

#define ON 0x01;
#define OFF 0x00;

#define OLEDLEFT 0
#define OLEDMID 64
#define OLEDRIGHT 127

//fundamental commands
#define OLED_INVERT 0xA6	//0 = off, 1 = on
#define OLED_CONTRAST 0X81
#define OLED_SLEEP 0XAE
#define OLED_WAKE 0XAF
#define OLED_COLUMNADDR 0x21
#define OLED_PAGEADDR 0x22

#define OLED 0x3C
#define OLED_DATA 0x40
#define OLED_COMMAND 0x00

void oled_write_int(unsigned char value);
//void oled_checkposx();
void oled_clear_ln(char page);
void oled_setpos(char x, char y);
void oled_fill();
void oled_clear();
void oled_write_char(char ch);
void oled_write_string(char *str);
void oled_control(char command);
void oled_data(char data);

#endif /* OLED_H_ */