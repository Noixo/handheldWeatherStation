#ifndef _I2C_H_
#define _I2C_H_

//Data line
#define SDA_HIGH  PORTB |= (1<<PB0)
#define SDA_LOW   PORTB &= ~(1<<PB0)

//Clock line
#define SCL_HIGH  PORTB |= (1<<PB2)
#define SCL_LOW   PORTB &= ~(1<<PB2)

#define TIME 10

#define READ 0x01
#define WRITE 0x00

extern void init_i2c();
extern void i2c_clock();
extern void i2c_start();
extern void i2c_stop();
extern void i2c_write(unsigned char byte);
extern char i2c_device_id(char id, char rw);
extern void i2c_read_id();
extern unsigned char i2c_read(char last_byte);

#endif