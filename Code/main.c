
#include <avr/io.h>
#include <stdlib.h>

#include "timeout.h"
#include "rtc/rtc.h"
#include "lcd/nokia5110.h"

#define N_BUTTONS 4

uint8_t *idp; // Time storage pointer

void setup(void);
void clear_time(void);
void get_time(void);
void arduino_blink(void);
void on_button(const uint8_t button);
void scan_buttons(void);

int main(void)
{
	// Time storage
	// second minute hour day month year week
	uint8_t timedigits[7] = {0, 0, 0, 0, 0, 0, 0};
	idp = timedigits;
	
	uint8_t current_second_value = 0;
	uint64_t longtime = 0;
	
	setup();
	
	nokia_lcd_write_string("chisti govno", 1);
	nokia_lcd_render();
	
	for (;;) //  ;_;
	{
		arduino_blink();
		
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
		
		for (uint8_t i = 0; i < 10; ++i)
		{
			_delay_ms(20);
			scan_buttons();
		}
	}
	
	return 0;
}

void setup(void)
{
	DDRB = 0x00;
	DDRC = 0x00;
	DDRD = 0x00;
	
	PORTB = 0x00;
	PORTC = 0x00;
	PORTD = 0x00;
	
	// Shift-register
	DDRD |= 0b00001100;
	PORTD |= (1 << 4); // Pull-up for button-reading pin
	
	DDRC |= (1 << 3);  // Set sound pin as output
	PORTC &= (1 << 3); // Sound turned off
	
	I2CInit(); // I2C (for RTC)
	
	nokia_lcd_init(); // LCD screen
	
	clear_time();
	
	// Clock start
	uint8_t t_start;
	DS1307Read(0x00, &t_start);
	t_start &= ~(1 << 7); // Set 7th bit 0
	DS1307Write(0x00,t_start);
}

void arduino_blink(void)
{
	DDRB |= (1 << 5);
	PORTB |= (1 << 5);
	_delay_ms(5);
	PORTB &= ~(1 << 5);
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
	// Set 0 and move it through 16 positions,
	// checking every button, if it's pressed.
	
	// D2 data
	// D3 clock
	// D4 read back
	
	for (uint8_t i = 0; i < N_BUTTONS; ++ i)
	{
		// Data
		if (i) PORTD |= _BV(PD2);
		else PORTD &= ~_BV(PD2); // If position is first, set 0
		_delay_us(100);
		
		// clock high
		PORTD |= _BV(PD3);
		_delay_us(100);
		
		// Data and clock low
		PORTD &= 0b11110011;
		
		// If button pressed
		if (bit_is_clear(PIND, 4)) on_button(i);
	}
}

void on_button(const uint8_t button)
{
	char button_str[20];
	itoa(button, button_str, 10);
	nokia_lcd_set_cursor(10, 30);
	nokia_lcd_write_string("Button pressed: ", 1);
	nokia_lcd_write_string(button_str, 1);
	nokia_lcd_render();
	_delay_ms(1000);
	nokia_lcd_clear();
}
