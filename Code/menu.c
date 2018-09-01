
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
			}
			flags.button_pressed = 0;
			nokia_lcd_render();
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
			}
			flags.button_pressed = 0;
			nokia_lcd_render();
		}
		
		else if (flags.button_pressed == 1) // ok
		{
			MENU_CURSOR_FILL(0);
			nokia_lcd_render();
			_delay_ms(100);
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
			PORTC |= _BV(PC2);
			flags.button_pressed = 0;
			flags.menu_is_displayed = 0;
			flags.info_is_displayed = 0;
			nokia_lcd_clear();
			nokia_lcd_render();
			return;
		}
	}
}

uint8_t display_info(void)
{
	flags.info_is_displayed = 1;
	char num[22]; // long numbers
	
	nokia_lcd_clear();
	
	nokia_lcd_set_cursor(3, 3);
	ultoa(longtime, num, 16);
	nokia_lcd_write_string("points: ", 1);
	nokia_lcd_write_string(num, 1);
	
	nokia_lcd_set_cursor(3, 12); // 7 pixels for character + 2 free pixels
	ultoa(state->level, num, 10);
	nokia_lcd_write_string("level: ", 1);
	nokia_lcd_write_string(num, 1);
	
	nokia_lcd_render();
	
	return 0;
}
