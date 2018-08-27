
#include <stdint.h>

// Values order in time array
// [second] [minute] [hour] [day] [month] [year] [week]
extern uint8_t *idp; // Time storage pointer

void clear_time(void);
void get_time(void);
