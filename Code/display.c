
#include "display.h"
#include "defines.h"
#include "lcd/nokia5110.h"
#include "creatures.h"

void display_creature(int level, unsigned frame, unsigned x_coord, unsigned y_coord)
{
	static int mem_level = -1;
	static uint8_t CREATUREFRAME_RAM[N_FRAMES_PER_CREATURE][BYTES_PER_CREATURE_FRAME];
	if (mem_level != level)
	{
		for (unsigned int i = 0; i < N_FRAMES_PER_CREATURE; ++i)
		{
			for (unsigned int j = 0; j < BYTES_PER_CREATURE_FRAME; ++j)
			{
				CREATUREFRAME_RAM[i][j] = pgm_read_byte(&CREATUREFRAME[level % N_LEVELS][i][j]);
			}
		}
		
		mem_level = level;
	}
	
	clear_creature_region(x_coord, y_coord);
	const uint8_t* img = CREATUREFRAME_RAM[frame % N_FRAMES_PER_CREATURE];
	unsigned int bit_counter;
	for (unsigned int i = 0; i < BYTES_PER_CREATURE_FRAME; ++i) // Iterate through bytes
	{
		bit_counter = 0;
		for (uint8_t j = 0x80; j > 0; j >>= 1)
		{
			nokia_lcd_set_pixel(x_coord + ((i % 4)*8) + bit_counter, y_coord + (i / 4), img[i] & j);
			++bit_counter;
		}
	}
}

void clear_creature_region(unsigned x_coord, unsigned y_coord)
{
	lcd_set_region(0, x_coord, y_coord, x_coord + CREATURE_FRAME_X - 1, y_coord + CREATURE_FRAME_Y - 1);
}

void lcd_set_region(unsigned value, unsigned x1, unsigned y1, unsigned x2, unsigned y2)
{
	for (unsigned int i = 0; i <= x2-x1; ++i)
		for (unsigned int j = 0; j <= y2-y1; ++j)
			nokia_lcd_set_pixel(x1 + i, y1 + j, value);
}

void display_picture_transparent(const uint8_t* pgm_bytes, unsigned x, unsigned y, unsigned frame_x, unsigned frame_y)
{
	unsigned int column_counter_x = 0;
	unsigned int row_counter_y = 0;
	const unsigned n_bytes = frame_x * frame_y / 8;
	for (unsigned int i = 0; i < n_bytes; ++i) // Iterate through bytes
	{
		for (uint8_t j = 0x80; j > 0; j >>= 1) // iterate through bits
		{
			if (
				(pgm_read_byte(&pgm_bytes[i]) & j) &&
				(x + column_counter_x < SCREEN_X) &&
				(y + row_counter_y < SCREEN_Y)
			) nokia_lcd_set_pixel(x + column_counter_x, y + row_counter_y, 1);
			++column_counter_x;
			if (column_counter_x >= frame_x)
			{
				column_counter_x = 0;
				++row_counter_y;
				if (row_counter_y >= frame_y) row_counter_y = frame_y-1;
			}
		}
	}
}

void display_picture(const uint8_t* pgm_bytes, unsigned x, unsigned y, unsigned frame_x, unsigned frame_y)
{
	lcd_set_region(0, x, y, x + frame_x - 1, y + frame_y - 1);
	display_picture_transparent(pgm_bytes, x, y, frame_x, frame_y);
}

void bad_parameter_signal(int param)
{
	switch (param)
	{
		case 0:
		{
			LED_RED_OFF;
			break;
		}
		
		default:
		{
			LED_RED_ON;
			break;
		}
	};
}
