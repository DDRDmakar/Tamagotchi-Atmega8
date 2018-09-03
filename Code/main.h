
#ifndef __MAIN
#define __MAIN

#define MAX_STATE_VALUE 18
#define MIN_STATE_VALUE -18

typedef struct Tama_state
{
	// Longtime is global
	unsigned int level; // Tamagochi creature level (from 0 to N)
	// Next level time = current level time + current level time / 2
	uint64_t cur_level; // Current level points
	uint64_t prev_level; // Previous level time
	
	
	int health; // Creature health (from -18 to 18)               -18=dead            0=ill     18=healthy
	int food;   // Creature hunger level (from -18 to 18)         -18=dead            0=hungry  18=fed
	int mood;   // Creature mood level (from -18 to 18)           -18=zero_level_time 0=sad     18=happy
	int cleanness; // Creature cleanness level (from -18 to 18)   -18=ill             0=dirty   18=clean
	
} Tama_state;

#define INIT_TAMA_STATE \
{ \
	.health =    11, \
	.food =      11, \
	.mood =      11, \
	.cleanness = 11, \
	 \
	.level = 0, \
	.cur_level = 0, \
	.prev_level = 0 \
}

typedef struct Flags
{
	uint8_t
	info_is_displayed,
	button_pressed,
	menu_is_displayed,
	parameter_is_bad;
} Flags;

extern uint64_t longtime;
extern Flags flags;
extern Tama_state* state;

#endif
