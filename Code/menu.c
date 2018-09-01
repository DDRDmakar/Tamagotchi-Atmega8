
// Functions handling menu list
// (configuring, rendering, buttons)

#include <string.h>
#include <stdlib.h>
#include <util/delay.h>

#include "menu.h"
#include "main.h"
#include "graphics.h"
#include "display.h"
#include "lcd/nokia5110.h"

void display_happy_creature(void);

void menu_render(void)
{
	display_picture(GRAPHICS_FULL[GRAPHICS_MENU_BGROUND_INDEX], 0, 0, 80, 48);
	nokia_lcd_render();
}

#define MENU_LINE_X 6
#define MENU_LINE_Y 10
#define MENU_LINE_STEP 12

void menu_render_letters(Menu* menu, unsigned int* top_position)
{
	for (uint8_t i = 0; i < 3; ++i)
	{
		unsigned int child_index = i + *top_position;
		if (child_index >= menu->n_children) child_index = menu->n_children-1;
		
		nokia_lcd_set_cursor(MENU_LINE_X, MENU_LINE_Y + i * MENU_LINE_STEP);
		
		uint8_t linelen = strlen(menu->children[child_index].text);
		for (int j = 0; j < 11; ++j)
		{
			if (j >= linelen) nokia_lcd_write_char(' ', 1);
			else nokia_lcd_write_char(menu->children[child_index].text[j], 1);
		}
	}
	nokia_lcd_render();
}

#define MENU_CURSOR_X 76
#define MENU_CURSOR_Y 11
#define MENU_CURSOR_STEP 12

#define MENU_CURSOR_FILL(cursor_fill_value) lcd_set_region((cursor_fill_value), MENU_CURSOR_X, MENU_CURSOR_Y + cursor * MENU_CURSOR_STEP, MENU_CURSOR_X + 3, MENU_CURSOR_Y + cursor * MENU_CURSOR_STEP + 3)

void on_menu(Menu* menu)
{
	flags.button_pressed = 0;
	
	menu_render();
	
	unsigned int cursor = 0;
	unsigned int top_position = 0;
	
	menu_render_letters(menu, &top_position);
	
	for(;;)
	{
		_delay_ms(10);
		
		if (flags.info_is_displayed && flags.button_pressed == 0) continue;
		if (flags.info_is_displayed && flags.button_pressed != 0)
		{
			flags.info_is_displayed = 0;
			flags.menu_is_displayed = 0;
			flags.button_pressed = 0;
			nokia_lcd_clear();
			nokia_lcd_render();
			return;
		}
		
		if (flags.button_pressed == 2) // up
		{
			if (cursor || top_position)
			{
				if (cursor)
				{
					MENU_CURSOR_FILL(0);
					--cursor;
					MENU_CURSOR_FILL(1);
				}
				else
				{
					--top_position;
					menu_render_letters(menu, &top_position);
				}
				nokia_lcd_render();
			}
			flags.button_pressed = 0;
		}
		
		
		else if (flags.button_pressed == 4) // down
		{
			if (!(cursor == 2 && top_position == menu->n_children - 3))
			{
				if (cursor != 2)
				{
					MENU_CURSOR_FILL(0);
					++cursor;
					MENU_CURSOR_FILL(1);
				}
				else
				{
					++top_position;
					menu_render_letters(menu, &top_position);
				}
				nokia_lcd_render();
			}
			flags.button_pressed = 0;
		}
		
		else if (flags.button_pressed == 1) // ok
		{
			MENU_CURSOR_FILL(0); // Cursor blink
			nokia_lcd_render();
			_delay_ms(300);
			MENU_CURSOR_FILL(1);
			nokia_lcd_render();
			
			flags.button_pressed = 0;
			
			if (menu->children[top_position + cursor].f != NULL && menu->children[top_position + cursor].f())
			{
				flags.menu_is_displayed = 0;
				return;
			}
		}
		
		else if (flags.button_pressed == 8) // menu
		{
			flags.button_pressed = 0;
			flags.menu_is_displayed = 0;
			flags.info_is_displayed = 0;
			nokia_lcd_clear();
			nokia_lcd_render();
			return;
		}
	}
}

#define FOOD_CURSOR_X 11
#define FOOD_CURSOR_Y 35
#define FOOD_CURSOR_STEP 27
#define FOOD_CURSOR_FILL(cursor_fill_value) lcd_set_region((cursor_fill_value), FOOD_CURSOR_X + cursor * FOOD_CURSOR_STEP, FOOD_CURSOR_Y, FOOD_CURSOR_X + cursor * FOOD_CURSOR_STEP + 3, FOOD_CURSOR_Y + 3)

uint8_t choose_food(void)
{
	flags.button_pressed = 0;
	flags.menu_is_displayed = 0;
	flags.info_is_displayed = 1;
	
	nokia_lcd_clear(); // Empty screen
	display_picture(GRAPHICS_FULL[GRAPHICS_FOOD_CHOICE_INDEX], 0, 0, 80, 48); // Draw food choice background
	nokia_lcd_render();
	
	unsigned int cursor = 0;
	
	for(;;)
	{
		_delay_ms(10);
		
		if (flags.button_pressed == 2) // left
		{
			if (cursor)
			{
				FOOD_CURSOR_FILL(0);
				--cursor;
				FOOD_CURSOR_FILL(1);
				nokia_lcd_render();
			}
			flags.button_pressed = 0;
		}
		
		
		else if (flags.button_pressed == 4) // right
		{
			if (cursor != 2)
			{
				FOOD_CURSOR_FILL(0);
				++cursor;
				FOOD_CURSOR_FILL(1);
				nokia_lcd_render();
			}
			flags.button_pressed = 0;
		}
		
		else if (flags.button_pressed == 1) // ok
		{
			FOOD_CURSOR_FILL(0); // Cursor blink
			nokia_lcd_render();
			_delay_ms(300);
			
			nokia_lcd_clear();
			nokia_lcd_render();
			
			// Feed creature norm
			
			display_happy_creature();
			
			goto _leave_food_choice;
		}
		
		else if (flags.button_pressed == 8) // menu
		{
			_leave_food_choice:
			flags.button_pressed = 0;
			flags.menu_is_displayed = 0;
			flags.info_is_displayed = 0;
			nokia_lcd_clear();
			nokia_lcd_render();
			return 1;
		}
	}
	
	nokia_lcd_render();
	
	return 1;
}


uint8_t display_info(void)
{
	flags.info_is_displayed = 1;
	char num[22]; // long numbers
	
	nokia_lcd_clear();
	
	nokia_lcd_set_cursor(3, 3);
	ultoa(longtime, num, 16);
	nokia_lcd_write_string("XP: 0x", 1);
	nokia_lcd_write_string(num, 1);
	
	nokia_lcd_set_cursor(3, 12); // 7 pixels for character + 2 free pixels
	ultoa(state->level, num, 10);
	nokia_lcd_write_string("level: ", 1);
	nokia_lcd_write_string(num, 1);
	
	nokia_lcd_render();
	
	return 0;
}

uint8_t display_shower(void)
{
	nokia_lcd_clear(); // Empty screen
	display_creature(state->level, 0, 27, 15); // Draw creature on the floor
	display_picture_transparent(GRAPHICS_FULL[GRAPHICS_SHOWER_INDEX], 0, 0, 80, 48); // Draw shower (transparent)
	
	nokia_lcd_render();
	_delay_ms(3000); // Display shower for 3 seconds
	nokia_lcd_clear(); // Empty screen
	
	display_happy_creature();
	
	return 1; // Leave menu
}

void display_happy_creature(void)
{
	for (int i = 0; i < 4; ++i)
	{
		clear_creature_region(27, 10);
		display_creature(state->level, 0, 27, 15); // Draw creature on the floor
		nokia_lcd_render();
		_delay_ms(200);
		
		clear_creature_region(27, 15);
		display_creature(state->level, 0, 27, 10); // Draw creature on the floor
		nokia_lcd_render();
		_delay_ms(200);
	}
	
	clear_creature_region(27, 10);
	display_creature(state->level, 0, 27, 15); // Draw creature on the floor
	nokia_lcd_render();
	_delay_ms(200);
	
	nokia_lcd_clear();
	
	flags.button_pressed = 0;
}

uint8_t display_healing(void)
{
	nokia_lcd_clear(); // Empty screen
	
	for (int i = 0; i < 10; ++i)
	{
		display_creature(state->level, 0, 27, 15); // Draw creature on the floor
		display_picture_transparent(GRAPHICS_FULL[GRAPHICS_STARS_INDEX], (i % 2 ? 0 : 4), 0, 80, 48); // Draw shower (transparent)
		nokia_lcd_render();
		_delay_ms(200); // Display shower for 3 seconds
		nokia_lcd_clear(); // Empty screen
	}
	
	display_happy_creature();
	
	return 1; // Leave menu
}

#define PLAY_DRAW_BALL(play_ball_value) \
	nokia_lcd_set_pixel(ball_x+1, ball_y, (play_ball_value)); \
	nokia_lcd_set_pixel(ball_x+2, ball_y, (play_ball_value)); \
	nokia_lcd_set_pixel(ball_x, ball_y+1, (play_ball_value)); \
	nokia_lcd_set_pixel(ball_x, ball_y+2, (play_ball_value)); \
	nokia_lcd_set_pixel(ball_x+3, ball_y+1, (play_ball_value)); \
	nokia_lcd_set_pixel(ball_x+3, ball_y+2, (play_ball_value)); \
	nokia_lcd_set_pixel(ball_x+1, ball_y+3, (play_ball_value)); \
	nokia_lcd_set_pixel(ball_x+2, ball_y+3, (play_ball_value));

#define PLAY_DRAW_CREATURE display_creature(state->level, 0, creature_x, creature_y) // Draw creature on the floor
#define PLAY_REMOVE_CREATURE clear_creature_region(creature_x, creature_y)

uint8_t play_with_creature(void)
{
	flags.button_pressed = 0;
	flags.menu_is_displayed = 0;
	flags.info_is_displayed = 1;
	
	unsigned int creature_x = 26;
	unsigned int ball_x     = 40;
	const unsigned int creature_y = 11;
	const unsigned int ball_y     = 44;
	
	nokia_lcd_clear(); // Empty screen
	PLAY_DRAW_BALL(1)
	PLAY_DRAW_CREATURE;
	nokia_lcd_render();
	
	unsigned int counter = 0;
	unsigned int counter2 = 0;
	
	for(;;)
	{
		_delay_ms(10);
		
		if (flags.button_pressed == 2) // left
		{
			if (ball_x >= 4)
			{
				PLAY_DRAW_BALL(0)
				ball_x -= 4;
				PLAY_DRAW_BALL(1)
				nokia_lcd_render();
			}
			flags.button_pressed = 0;
		}
		
		
		else if (flags.button_pressed == 4) // right
		{
			if (ball_x <= 76) // 84 - 4 - 4 = 76
			{
				PLAY_DRAW_BALL(0);
				ball_x += 4;
				PLAY_DRAW_BALL(1);
				nokia_lcd_render();
			}
			flags.button_pressed = 0;
		}
		
		else if (flags.button_pressed == 1 || flags.button_pressed == 8) // menu and ok
		{
			_leave_creature_play:
			flags.button_pressed = 0;
			flags.menu_is_displayed = 0;
			flags.info_is_displayed = 0;
			nokia_lcd_clear();
			nokia_lcd_render();
			return 1;
		}
		
		if (counter % 16 == 0) // Move creature
		{
			int delta_x = (ball_x + 1) - (creature_x + 16);
			
			if (delta_x > 0 && creature_x < 50) // 84 - 32 - 2 = 50
			{
				// Move creature 2 pixels right
				PLAY_REMOVE_CREATURE;
				++creature_x;
				PLAY_DRAW_CREATURE;
				nokia_lcd_render();
				if (state->mood < 12 && (counter2 % 10 == 0)) ++state->mood;
				++counter2;
			}
			else if (delta_x < 0 && creature_x >= 2)
			{
				// Move creature 2 pixels left
				PLAY_REMOVE_CREATURE;
				--creature_x;
				PLAY_DRAW_CREATURE;
				nokia_lcd_render();
				if (state->mood < 12 && (counter2 % 10 == 0)) ++state->mood;
				++counter2;
			}
			
			if (state->mood >= 12)
			{
				nokia_lcd_clear();
				display_happy_creature();
				goto _leave_creature_play;
			}
		}
		
		++counter;
	}
	
	nokia_lcd_render();
	
	return 1;
}
