
#include <avr/io.h>
#include <stdlib.h>

#include "timeout.h"
#include "rtc/rtc.h"
#include "lcd/nokia5110.h"

#define N_BUTTONS 4
#define MAX_CREATURE_LEVEL 12


// Values order in time array
// [second] [minute] [hour] [day] [month] [year] [week]
uint8_t *idp;       // Time storage pointer
uint64_t longtime;  // Absolute seconds count

unsigned int level; // Tamagochi creature level (from 0 to N)

int health; // Creature health (from -6 to 6)
int food;   // Creature hunger level (from -6 to 6) -6=hungry 6=fed
int mood;   // Creature mood level (from -6 to 6)


void setup(void);      // Set all values and ports at start
void clear_time(void); // Set time to zeros
void get_time(void);   // Read all time values to *idp
uint8_t get_seconds(void);
uint8_t get_hours(void);
void arduino_blink(void);
void on_button(const uint8_t button);
uint8_t scan_buttons(void);
void alarm_off(void);
void alarm_on(void);
void clock_stop(void);
void clock_start(void);


int main(void)
{
	// Time storage
	// second minute hour day month year week
	uint8_t timedigits[7] = {0, 0, 0, 0, 0, 0, 0};
	idp = timedigits;
	
	uint8_t current_second_value = 0;
	
	setup();
	
	nokia_lcd_write_string("chisti govno", 1);
	nokia_lcd_render();
	
	// Loops 5 times per second
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
		
		
		// =================== DEBUG
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
		
		// =================== DEBUG
		
		// Scan buttons 1 time per 20 ms
		for (uint8_t i = 0; i < 10; ++i)
		{
			_delay_ms(20);
			if (scan_buttons()) break;
		}
		
		if (health == -6) /*on_illness()*/;
		
		if (mood < 0) /*on_sad()*/;
		
		if (food < 0) /*on_hunger()*/;
		
		
		nokia_lcd_render();
	}
	
	return 0;
}

void setup(void)
{
	DDRB = 0x00; // PORTB input
	DDRC = 0x00; // PORTC input
	DDRD = 0x00; // PORTD input
	
	PORTB = 0x00; // PORTB LOW
	PORTC = 0x00; // PORTC LOW
	PORTD = 0x00; // PORTD LOW
	
	// Shift-register
	DDRD |= 0b00001100; // Set PORTD pins 2 and 3 as outputs
	PORTD |= (1 << 4);  // Pull-up for button-reading pin PORTD.4
	
	DDRC |= (1 << 3);  // Set sound pin as output
	alarm_off(); // Sound turned off
	
	I2CInit(); // I2C (for RTC)
	
	nokia_lcd_init(); // LCD screen
	
	clock_start(); // Start clock
	
	clear_time(); // Time on start is always 0
}

// DEBUG FUNCTION
void arduino_blink(void)
{
	DDRB |= (1 << 5);
	PORTB |= (1 << 5);
	_delay_ms(5);
	PORTB &= ~(1 << 5);
}

// Set time to 0, if it's not 0
void clear_time(void) { for (uint8_t i = 0; i < 7; ++i) DS1307Write(i, 0x00); }

// Read all time values
void get_time(void)
{
	*(idp + 5) = ds1307_get_dec(0x06); // year
	*(idp + 4) = ds1307_get_dec(0x05); // month
	*(idp + 3) = ds1307_get_dec(0x04); // day
	*(idp + 6) = ds1307_get_dec(0x03); // day of week
	*(idp + 2) = ds1307_get_dec(0x02); // hour
	*(idp + 1) = ds1307_get_dec(0x01); // min
	*(idp + 0) = ds1307_get_dec(0x00); // sec
}

uint8_t get_seconds(void)
{
	*idp = ds1307_get_dec(0x00); // sec
	return *idp;
}

uint8_t get_hours(void)
{
	*(idp + 2) = ds1307_get_dec(0x02); // hour
	return *(idp + 2);
}

// Scan all buttons
uint8_t scan_buttons(void)
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
		if (bit_is_clear(PIND, 4))
		{
			on_button(i);
			return 1;
		}
	}
	// Return false if no buttons pressed
	return 0;
}

void on_button(const uint8_t button)
{
	switch (button)
	{
		case 0: {alarm_on(); break;} // TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
		case 1: {alarm_off(); break;}
		case 2: {clock_start(); break;}
		case 3: {clock_stop(); break;}
		default: break;
	}
}

void alarm_off(void)
{
	PORTC &= ~(1 << 3);
}

void alarm_on(void)
{
	PORTC |= (1 << 3);
}

void clock_stop(void)
{
	uint8_t t_start;
	DS1307Read(0x00, &t_start);
	t_start |= (1 << 7); // Set 7th bit 1
	DS1307Write(0x00, t_start);
}

void clock_start(void)
{
	uint8_t t_start;
	DS1307Read(0x00, &t_start);
	t_start &= ~(1 << 7); // Set 7th bit 0
	DS1307Write(0x00, t_start);
}
