
#include <avr/eeprom.h>
#include "eeprom.h"
#include "main.h"

// Create variable in EEPROM with initial values
Tama_state EEMEM eestate = INIT_TAMA_STATE;
uint64_t EEMEM ee_longtime = 0;

uint8_t eeprom_save(void)
{
	eeprom_write_block(state, &eestate, sizeof(Tama_state));
	eeprom_write_block(&longtime, &ee_longtime, sizeof(uint64_t));
	return 0;
}

uint8_t eeprom_load(void)
{
	eeprom_read_block(state, &eestate, sizeof(Tama_state));
	eeprom_read_block(&longtime, &ee_longtime, sizeof(uint64_t));
	return 0;
}
