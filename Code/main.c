
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include "rtc/rtc.h"
#include "lcd/nokia5110.h"
#include "time.h"
#include "defines.h"


void setup(void);
// Values order in time array
// [second] [minute] [hour] [day] [month] [year] [week]
uint8_t *idp; // Time storage pointer

uint64_t longtime;  // Absolute seconds count

unsigned int level; // Tamagochi creature level (from 0 to N)

int health; // Creature health (from -6 to 6)
int food;   // Creature hunger level (from -6 to 6) -6=hungry 6=fed
int mood;   // Creature mood level (from -6 to 6)



int main(void)
{
	// Time storage
	// second minute hour day month year week
	uint8_t timedigits[7] = {0, 0, 0, 0, 0, 0, 0};
	idp = timedigits;
	
	setup();
	
	nokia_lcd_write_string("chisti govno", 1);
	nokia_lcd_render();
	
	// Loops 5 times per second
	for (;;) //  ;_;
	{
		// Increment seconds
		get_time();
		
		_delay_ms(1000);
		++longtime;
		
		
		// =================== DEBUG
		// Display time
		char ltm[20]; // 20 chars max to display timestamp
		itoa(longtime, ltm, 10);
		nokia_lcd_set_cursor(10, 20);
		nokia_lcd_write_string(ltm, 1);
		
		if (health == -6) /*on_illness()*/;
		
		if (mood < 0) /*on_sad()*/;
		
		if (food < 0) /*on_hunger()*/;
		nokia_lcd_render();
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
	DDRC =  0x37; // 00110111
	PORTC = 0x70; // 01110000
	
	// PORT D
	
	/* DDRD PORTD
	 * 1    0      D0 - Shift clock
	 * 1    0      D1 - Shift data
	 * 0    1      D2 - interrupt 0 (buttons)
	 * 0    1      D3 - interrupt 1 (clock)
	 * 1    0      D4 - Shift latch
	 * 0    0      D5 -       not used
	 * 1    0      D6 - PWM 0A (speaker)
	 * 1    0      D7 - Shift Reset
	 */
	DDRD =  0xD3; // 11010011
	PORTD = 0x0C; // 00001100
	
	I2CInit(); // I2C (for RTC)
	
	nokia_lcd_init(); // LCD screen
	
	// Clock start
	uint8_t t_start;
	DS1307Read(0x00, &t_start);
	t_start &= ~(1 << 7); // Set 7th bit 0
	DS1307Write(0x00, t_start);
	
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

