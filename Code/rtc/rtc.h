
#include <avr/io.h>
#include <util/delay.h> 

#ifndef __RTC
#define __RTC

#define F_SCL 100000L // Частота I2C 100 кГц

void I2CInit(void);
uint8_t DS1307Read(uint8_t address,uint8_t *data);
uint8_t DS1307Write(uint8_t address,uint8_t data);

uint8_t ds1307_get_dec(uint8_t address);

#endif
