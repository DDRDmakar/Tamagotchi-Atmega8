
#include <avr/pgmspace.h>

#ifndef __DISPLAY
#define __DISPLAY

void display_creature(unsigned level, unsigned frame, unsigned x_coord, unsigned y_coord);
void clear_creature_region(unsigned x_coord, unsigned y_coord);
void lcd_set_region(unsigned value, unsigned x1, unsigned y1, unsigned x2, unsigned y2);
void display_picture_transparent(const uint8_t* pgm_bytes, unsigned x, unsigned y, unsigned frame_x, unsigned frame_y);
void display_picture(const uint8_t* pgm_bytes, unsigned x, unsigned y, unsigned frame_x, unsigned frame_y);

#endif
