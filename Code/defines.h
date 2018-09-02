
#include <stdlib.h>

#ifndef __DEFINE
#define __DEFINE

// screen 84*48 px
#define SCREEN_X 84
#define SCREEN_Y 48

#define N_BUTTONS 4
#define MAX_TAMA_LEVEL 12
#define N_LEVELS 5
#define N_FRAMES_PER_CREATURE 3

#define RANDOM_ARRAY_SIZE 50
#define RANDOM_ARRAY_RANGE 65536
extern const uint16_t* RAND;

// State changing period in seconds
#define STATE_PERIOD_HEALTH     202 // Independent
#define STATE_PERIOD_FOOD       50  // Refers to health and mood
#define STATE_PERIOD_MOOD       99  // Refers to health
#define STATE_PERIOD_CLEANNESS  104 // Refers to health and mood

#define MIN_BAD_THRESHOLD_HEALTH    -12
#define MIN_BAD_THRESHOLD_FOOD      -12
#define MIN_BAD_THRESHOLD_MOOD      -14
#define MIN_BAD_THRESHOLD_CLEANNESS -14

#define LED_RED_ON  PORTC |=  _BV(PC0)
#define LED_RED_OFF PORTC &= ~_BV(PC0)
#define LED_BLUE_ON  PORTC |=  _BV(PC1)
#define LED_BLUE_OFF PORTC &= ~_BV(PC1)
#define LED_WHITE_ON  PORTC |=  _BV(PC2)
#define LED_WHITE_OFF PORTC &= ~_BV(PC2)

#endif
