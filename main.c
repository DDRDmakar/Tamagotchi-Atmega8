
#include <avr/io.h>
#include <stdlib.h>

#include "timeout.h"
#include "rtc/rtc.h"
#include "lcd/nokia5110.h"

uint8_t *idp; // Time storage pointer

void setup(void);
void clear_time(void);
void get_time(void);

int main(void)
{
	// Time storage
	// second minute hour day month year week
	uint8_t timedigits[7] = {0, 0, 0, 0, 0, 0, 0};
	idp = timedigits;
	
	uint8_t current_second_value = 0;
	uint64_t longtime = 0;
	
	setup();
	
	for (;;) //  ;_;
	{
		// Increment seconds
		get_time();
		if (current_second_value != *idp)
		{
			++longtime;
			current_second_value = *idp;
		}
		
		// Display time
		nokia_lcd_set_cursor(10, 10);
		nokia_lcd_write_char('0' + *(idp + 2) / 10, 1);
		nokia_lcd_write_char('0' + *(idp + 2) % 10, 1);
		nokia_lcd_write_char(':', 1);
		nokia_lcd_write_char('0' + *(idp + 1) / 10, 1);
		nokia_lcd_write_char('0' + *(idp + 1) % 10, 1);
		nokia_lcd_write_char(':', 1);
		nokia_lcd_write_char('0' + *(idp + 0) / 10, 1);
		nokia_lcd_write_char('0' + *(idp + 0) % 10, 1);
		char ltm[20];
		itoa(longtime, ltm, 10);
		nokia_lcd_set_cursor(10, 20);
		nokia_lcd_write_string(ltm, 1);
		
		nokia_lcd_render();
		
		_delay_ms(200);
	}
	
	for (;;)
	{
		nokia_lcd_set_cursor(10, 35);
		get_time();
		
		nokia_lcd_write_char('0' + *(idp + 2) / 10, 1);
		nokia_lcd_write_char('0' + *(idp + 2) % 10, 1);
		nokia_lcd_write_char(':', 1);
		nokia_lcd_write_char('0' + *(idp + 1) / 10, 1);
		nokia_lcd_write_char('0' + *(idp + 1) % 10, 1);
		nokia_lcd_write_char(':', 1);
		nokia_lcd_write_char('0' + *(idp + 0) / 10, 1);
		nokia_lcd_write_char('0' + *(idp + 0) % 10, 1);
		_delay_ms(100);
    }
	
	return 0;
}

void setup(void)
{
	DDRB = 0xFF;
	DDRC = 0xFF;
	DDRD = 0xFF;
	
	PORTB = 0x00;
	PORTC = 0x00;
	PORTD = 0x00;
	
	I2CInit(); // I2C
	/*
	DDRC = 0x00;		//Эта бурда нужна для
	DDRD &= ~_BV(1);	//приведения в порядок
	DDRD &= ~_BV(3);	//портов МК после инициализации
	
	PORTC = 0xFF;
	PORTD |= _BV(1);
	PORTD |= _BV(3);	//Конец бурды
	*/
	
	clear_time();
	
	// Clock start
	uint8_t t_start;
	DS1307Read(0x00, &t_start);
	t_start &= ~(1 << 7); // Set 7th bit 0
	DS1307Write(0x00,t_start);
}

// Set time to 0, if it's not 0
void clear_time(void) { for (uint8_t i = 0; i < 7; ++i) if (!DS1307Write(i, 0x00)) break; }

// Read all time values
void get_time(void)
{
	*(idp + 0) = ds1307_get_dec(0x00); // sec
	*(idp + 1) = ds1307_get_dec(0x01); // min
	*(idp + 2) = ds1307_get_dec(0x02); // hour
	
	*(idp + 6) = ds1307_get_dec(0x03); // day of week
	
	*(idp + 3) = ds1307_get_dec(0x04); // day
	*(idp + 4) = ds1307_get_dec(0x05); // month
	*(idp + 5) = ds1307_get_dec(0x06); // year
}

// Scan all buttons
void scan_buttons(void)
{
	// Открываю latch
	// (C0 HIGH)
	PORTC |= _BV(PC0);
	_delay_us(500);
	
	// Set 0 and move it through 16 positions,
	// checking every button, if it's pressed.
	for (uint8_t i = 0; i < 16; ++ i)
	{
		// Data
		if (i) PORTC |= _BV(PC2);
		else PORTC &= ~_BV(PC2); // If position is first, set 0
		_delay_us(200);
		
		// clock high
		PORTC |= _BV(PC1);
		_delay_us(200);
		
		// Data and clock low
		PORTC &= 0b11111001;
		
		// If button pressed
		if (PORTD/*<inpit pin> is clear*/) /*on_button(i)*/;
	}
	
	// Закрываю latch
	// (B2 LOW)
	PORTC |= _BV(PC0);
	_delay_us(500);
}
