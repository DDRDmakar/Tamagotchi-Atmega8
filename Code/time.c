
#include "time.h"
#include "rtc/rtc.h"

// Set time to 0, if it's not 0
void clear_time(void) { for (uint8_t i = 0; i < 7; ++i) if (!DS1307Write(i, 0x00)) break; }

// Read all time values
void get_time(void)
{
	*(idp + 0) = ds1307_get_dec(0x00); // sec
	*(idp + 1) = ds1307_get_dec(0x01); // min
	*(idp + 2) = ds1307_get_dec(0x02); // hour
	
	*(idp + 6) = ds1307_get_dec(0x03); // day of week
	
	*(idp + 3) = ds1307_get_dec(0x04); // day
	*(idp + 4) = ds1307_get_dec(0x05); // month
	*(idp + 5) = ds1307_get_dec(0x06); // year
}
