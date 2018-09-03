

MMCU_GCC     = atmega328p
MMCU_AVRDUDE = m328p
DEVICE    = /dev/ttyUSB0
INTERFACE = stk500v1
SPEED     = 19200
FREQ_CPU  = 16000000

HFUSE = 0xDD
LFUSE = 0xFF
EFUSE = 0x00

CC   = avr-gcc
DIR  = ../Code/
CFLAGS := -mmcu=$(MMCU_GCC) -Os -pedantic -Wextra -Wall -std=c11 -DF_CPU=$(FREQ_CPU)UL
CFLAGS_AVRDUDE = -c $(INTERFACE) -p $(MMCU_AVRDUDE) -P $(DEVICE) -b $(SPEED)

prog: main.o lcd.o rtc.o time.o display.o menu.o eeprom.o
	$(CC) $(CFLAGS) -o prog main.o lcd.o rtc.o time.o display.o menu.o eeprom.o

main.o: $(DIR)main.c $(DIR)main.h $(DIR)defines.h $(DIR)rtc/rtc.h $(DIR)lcd/nokia5110.h $(DIR)display.h $(DIR)menu.h $(DIR)time.h $(DIR)eeprom.h
	$(CC) $(CFLAGS) -c $(DIR)main.c

rtc.o: $(DIR)rtc/rtc.c $(DIR)rtc/rtc.h
	$(CC) $(CFLAGS) -c $(DIR)rtc/rtc.c
lcd.o: $(DIR)lcd/lcd.c $(DIR)lcd/nokia5110.h $(DIR)lcd/nokia5110_chars.h
	$(CC) $(CFLAGS) -c $(DIR)lcd/lcd.c
time.o: $(DIR)time.c $(DIR)time.h $(DIR)rtc/rtc.h
	$(CC) $(CFLAGS) -c $(DIR)time.c
display.o: $(DIR)display.c $(DIR)display.h $(DIR)lcd/nokia5110.h $(DIR)defines.h $(DIR)creatures.h
	$(CC) $(CFLAGS) -c $(DIR)display.c
menu.o: $(DIR)menu.c $(DIR)menu.h $(DIR)main.h $(DIR)lcd/nokia5110.h $(DIR)defines.h $(DIR)display.h $(DIR)graphics.h
	$(CC) $(CFLAGS) -c $(DIR)menu.c
eeprom.o: $(DIR)eeprom.c $(DIR)eeprom.h $(DIR)main.h
	$(CC) $(CFLAGS) -c $(DIR)eeprom.c

hex: prog
	avr-size -C --mcu=$(MMCU_GCC) prog
	avr-objcopy -R .eeprom -O ihex prog prog.hex
upload: hex
	avrdude $(CFLAGS_AVRDUDE) -U flash:w:prog.hex

lfuse:
	avrdude $(CFLAGS_AVRDUDE) -U lfuse:w:$(LFUSE):m
hfuse:
	avrdude $(CFLAGS_AVRDUDE) -U hfuse:w:$(HFUSE):m
efuse:
	avrdude $(CFLAGS_AVRDUDE) -U efuse:w:$(EFUSE):m

fuses: lfuse hfuse efuse

clean:
	rm *.o

cp:
	cp prog.hex /media/STOPKA/VMs/SharedFolder/AVR/Tamagotchi/
