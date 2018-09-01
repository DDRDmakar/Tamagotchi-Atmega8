
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include "rtc/rtc.h"
#include "lcd/nokia5110.h"
#include "time.h"
#include "main.h"
#include "defines.h"
#include "display.h"
#include "menu.h"

const uint16_t* RAND;

uint8_t *idp; // Time storage pointer
uint64_t longtime; // Absolute seconds count
Flags flags = {0, 0, 0};
Menu* menu_main;
Tama_state* state;

void setup(void);
uint8_t check_buttons_pressed(void);
void shift_in_bit(uint8_t b);
void shift_in_8bit(uint8_t b);
void shift_clear(void);


int main(void)
{
	// Random numbers
	const uint16_t randarr[RANDOM_ARRAY_SIZE] = {57925, 23685, 36358, 39954, 25669, 9664, 55594, 8926, 3764, 61085, 35285, 57944, 20421, 52830, 50818, 20167, 32015, 1634, 43969, 64642, 29894, 39401, 15811, 45388, 25878, 530, 11598, 54822, 34968, 64956, 5101, 24209, 36461, 22856, 21265, 33972, 23488, 58118, 26648, 38519, 1651, 47372, 18825, 2863, 64657, 47348, 12934, 40169, 65054, 48949 };
	RAND = randarr;
	
	Menu menu_main_children[4] = {
		{
			.action = 0,
			.text = "Info",
			.children = NULL,
			.n_children = 0,
			.f = display_info
		},
		{
			.action = 0,
			.text = "Feed me",
			.children = NULL,
			.n_children = 0,
			.f = NULL
		},
		{
			.action = 0,
			.text = "Wash me",
			.children = NULL,
			.n_children = 0,
			.f = NULL
		},
		{
			.action = 0,
			.text = "Heal me",
			.children = NULL,
			.n_children = 0,
			.f = NULL
		}
	};
	
	// Create menu structure
	Menu menu =
	{
		.action = 1,
		.text = "",
		.children = menu_main_children,
		.n_children = 4,
		.f = NULL
	};
	menu_main = &menu;
	
	longtime = 0;
	
	// Time storage
	// second minute hour day month year week
	uint8_t timedigits[7] = {0, 0, 0, 0, 0, 0, 0};
	idp = timedigits;
	
	Tama_state tama_state;
	state = &tama_state;
	state->level = 3;
	
	setup();
	
	// Loops 5 times per second
	for (;;) //  ;_;
	{
		// get_time();
		
		unsigned int x = RAND[longtime % RANDOM_ARRAY_SIZE] % 52;
		unsigned int y = RAND[longtime % RANDOM_ARRAY_SIZE] % 16;
		
		int j = 0;
		uint8_t direction = 0;
		
		for(unsigned int i = 0; i < 19; ++i)
		{
			if (!flags.info_is_displayed && !flags.menu_is_displayed)
			{
				if (!flags.info_is_displayed && !flags.menu_is_displayed) display_creature(state->level, j, x, y);
				if (direction) // Reverse loop
				{
					if (j > 0) --j;
					if (j <= 0) direction = 0;
				}
				else
				{
					if (j < 2) ++j;
					if (j >= 2) direction = 1;
				}
				
				if (!flags.info_is_displayed && !flags.menu_is_displayed) nokia_lcd_render();
			}
			
			if (flags.menu_is_displayed) on_menu(menu_main);
			
			if (i == 18) // Loop forever
			{
				i = 0;
				if (!flags.info_is_displayed && !flags.menu_is_displayed) clear_creature_region(x, y);
			}
			
			_delay_ms(500);
		}
		
		_delay_ms(100);
	}
	
	return 0;
}

void setup(void)
{
	// PORT B
	
	/* DDRB PORTB
	 * 0    0      B0 - LCD SCE
	 * 0    0      B1 - LCD RST
	 * 0    0      B2 - LCD D/C
	 * 0    0      B3 - LCD D/N
	 * 0    0      B4 - LCD SCLK
	 * 0    0      B5 -      not used
	 * 0    0      B6 -      not used
	 * 0    0      B7 -      not used
	 */
	DDRB =  0x00;
	PORTB = 0x00;
	
	// PORT C
	
	/* DDRC PORTC
	 * 1    0      C0 - led 0
	 * 1    0      C1 - led 1
	 * 1    0      C2 - led 2
	 * 0    0      C3 -        not used
	 * 1    1      C4 - DS-1307 SDA
	 * 1    1      C5 - DS-1307 SCL
	 * 0    1      C6 - ATMEGA RESET
	 * 0    0      C7 -        not used
	 */
	DDRC =  0x37;
	PORTC = 0x70;
	
	// PORT D
	
	/* DDRD PORTD
	 * 1    0      D0 - Shift clock
	 * 1    0      D1 - Shift data
	 * 0    1      D2 - interrupt 0 (buttons)
	 * 0    1      D3 - interrupt 1 (clock)
	 * 1    0      D4 - Shift latch
	 * 0    0      D5 -       not used
	 * 1    0      D6 - PWM 0A (speaker)
	 * 1    1      D7 - Shift Reset
	 */
	DDRD =  0xD3;
	PORTD = 0x8C;
	
	_delay_ms(100);
	
	nokia_lcd_init(); // LCD screen
	
	sei(); //  Enable global interrupts
	
	/*****************TODO RTC***********************/
	
	/*
	I2CInit(); // I2C (for RTC)
	
	// Clock start
	uint8_t t_start;
	DS1307Read(0x00, &t_start);
	t_start &= ~(1 << 7); // Set 7th bit 0
	DS1307Write(0x00, t_start);
	*/
	
	/************************************************/
	
	// Internal interrupt instead of RTC
	
	TCCR1A = 0;
	TCCR1B |= (1 << WGM12); // CTC timer mode
	TIMSK1 |= (1 << OCIE1A); // Enable interrupts
	OCR1AH = 0x3D; // Number to compare with
	OCR1AL = 0x09; // 16000000 / 1024 = 15625 = 0x3D09
	TCCR1B |= (1 << CS10)|(1 << CS12); // divide F_CPU by 1024
	
	
	/************************************************/
	
	EIMSK |= (1 << INT0);   // Allow interrupt INT0 for buttons
	EICRA |= (1 << ISC01); // Interrupt on falling edge INT0
	
	shift_in_8bit(0x00);
	
	DDRD &= ~(1 << 2);
	DDRD &= ~(1 << 3);
	PORTD |= (1 << 2);
	PORTD |= (1 << 3);
	
	/************************************************/
	
	// Blink leds on startup
	_delay_ms(250);
	PORTC |= _BV(PC0);
	_delay_ms(250);
	PORTC |= _BV(PC1);
	_delay_ms(250);
	PORTC |= _BV(PC2);
	_delay_ms(250);
	PORTC &= ~_BV(PC0);
	PORTC &= ~_BV(PC1);
	PORTC &= ~_BV(PC2);
	_delay_ms(250);
}

ISR (TIMER1_COMPA_vect) // Every second interrupt
{
	// Increment time variable
	++longtime;
	
	// blink led 1
	PORTC |= _BV(PC1);
	_delay_ms(1);
	PORTC &= ~_BV(PC1);
} 

ISR (INT0_vect) // On button pressed interrupt
{
	// Disable INT0 interrupt
	EIMSK &= ~(1 << INT0);
	
	uint8_t res = check_buttons_pressed();
	
	if (res != 0)
	{
		/*
		char ltm[4]; // integer from 0 to 255
		itoa(res, ltm, 10);
		nokia_lcd_set_cursor(10, 30);
		nokia_lcd_write_string("    ", 2);
		nokia_lcd_set_cursor(10, 30);
		nokia_lcd_write_string(ltm, 2);
		*/
		
		// blink led 2
		PORTC |= _BV(PC2);
		_delay_ms(100);
		PORTC &= ~_BV(PC2);
		
		switch (res)
		{
			case 1: // OK
			{
				flags.button_pressed = res;
				break;
			}
			case 2: // UP
			{
				flags.button_pressed = res;
				break;
			}
			case 4: // DOWN
			{
				flags.button_pressed = res;
				break;
			}
			case 8: // LEFT (MENU)
			{
				if (!flags.menu_is_displayed)
				{
					menu_render();
					nokia_lcd_render();
				}
				else flags.button_pressed = res;
				flags.menu_is_displayed = !flags.menu_is_displayed;
				break;
			}
			case 16: // RIGHT
			{
				break;
			}
		}
	}
	
	// Enable INT0 interrupt
	EIMSK |= (1 << INT0);
	// Clear INT0 flag
	EIFR = (1 << INTF0);
}

#define SHIFT_CLOCK 0 // positive edge
#define SHIFT_DATA  1
#define SHIFT_LATCH 4 // positive edge
#define SHIFT_RESET 7 // low level + clock

uint8_t check_buttons_pressed(void)
{
	uint8_t res;
	shift_in_8bit(0xFE);
	_delay_ms(1);
	res = bit_is_clear(PIND, 2) ? 0x01 : 0x00;
	
	for (uint8_t i = 1; i < 8; ++i)
	{
		shift_in_bit(1);
		_delay_ms(1);
		res |= (bit_is_clear(PIND, 2) << i);
	}
	shift_clear();
	_delay_ms(1);
	
	return res;
}

void shift_in_bit(uint8_t b)
{
	// Data
	if (b) PORTD |= (1 << SHIFT_DATA);
	else   PORTD &= ~(1 << SHIFT_DATA);
	// Clock high
	PORTD |= (1 << SHIFT_CLOCK);
	_delay_us(1);
	// Latch high
	PORTD |= (1 << SHIFT_LATCH);
	_delay_us(1);
	// Clock low
	PORTD &= ~(1 << SHIFT_CLOCK);
	// Latch low
	PORTD &= ~(1 << SHIFT_LATCH);
	_delay_us(1);
}

void shift_in_8bit(uint8_t b)
{
	for (uint8_t i = 0; i < 8; ++i)
	{
		_delay_us(1);
		// Data
		if (b & 0x80) PORTD |= (1 << SHIFT_DATA);
		else          PORTD &= ~(1 << SHIFT_DATA);
		// Clock high
		PORTD |= (1 << SHIFT_CLOCK);
		_delay_us(1);
		// Clock low
		PORTD &= ~(1 << SHIFT_CLOCK);
		b <<= 1;
	}
	// Latch high
	PORTD |= (1 << SHIFT_LATCH);
	_delay_us(1);
	// Latch low
	PORTD &= ~(1 << SHIFT_LATCH);
}

void shift_clear(void)
{
	// Reset low
	PORTD &= ~(1 << SHIFT_RESET);
	_delay_us(1);
	// Clock high
	PORTD |= (1 << SHIFT_CLOCK);
	_delay_us(1);
	// Latch high
	PORTD |= (1 << SHIFT_LATCH);
	_delay_us(1);
	// Clock low
	PORTD &= ~(1 << SHIFT_CLOCK);
	// Reset high
	PORTD |= (1 << SHIFT_RESET);
	// Latch low
	PORTD &= ~(1 << SHIFT_LATCH);
	_delay_us(1);
}
