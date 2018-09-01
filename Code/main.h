
#ifndef __MAIN
#define __MAIN

typedef struct Tama_state
{
	unsigned int level; // Tamagochi creature level (from 0 to N)
	
	unsigned int k; // Next level length coefficient
	uint64_t prev_level; // Previous level time
	
	int health; // Creature health (from -6 to 6)
	int food;   // Creature hunger level (from -6 to 6) -6=hungry 6=fed
	int mood;   // Creature mood level (from -6 to 6)
	int cleanness; // Creature cleanness level (from -12 to 12)
	
} Tama_state;

typedef struct Flags
{
	uint8_t
	info_is_displayed,
	button_pressed,
	menu_is_displayed;
} Flags;

extern uint64_t longtime;
extern Flags flags;
extern Tama_state* state;

#endif
