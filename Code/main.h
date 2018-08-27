
typedef struct Tama_state
{
	unsigned int level; // Tamagochi creature level (from 0 to N)

	int health; // Creature health (from -6 to 6)
	int food;   // Creature hunger level (from -6 to 6) -6=hungry 6=fed
	int mood;   // Creature mood level (from -6 to 6)
	
} Tama_state;

extern uint64_t longtime;
