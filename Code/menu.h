
#include <stdint.h>

#ifndef __MENU
#define __MENU

typedef struct Menu
{
	uint8_t action; // 0 - function; 1 - submenu
	
	char* text; // Text of menu node
	
	struct Menu* children; // Array of menu children
	unsigned n_children; // Children count
	
	uint8_t (*f)(void); // Function
	// return 0 - nothing; 1 - leave menu
	
} Menu;

void menu_render(void);
void on_menu(Menu* menu);
void die(void);

uint8_t display_info(void);
uint8_t display_shower(void);
uint8_t choose_food(void);
uint8_t display_healing(void);
uint8_t play_with_creature(void);

#endif
