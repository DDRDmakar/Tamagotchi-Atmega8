

MMCU_GCC=atmega328p
MMCU_AVRDUDE=m328p
DEVICE=/dev/ttyUSB0
INTERFACE=stk500v1
SPEED=19200
FREQ_CPU=8000000

CC=avr-gcc
DIR=../Code/
OPT := -mmcu=$(MMCU_GCC) -Os -std=c99 -DF_CPU=$(FREQ_CPU)UL

prog: main.o lcd.o rtc.o
	$(CC) $(OPT) -o prog main.o lcd.o rtc.o

main.o: $(DIR)main.c $(DIR)timeout.h
	$(CC) $(OPT) -c $(DIR)main.c

rtc.o: $(DIR)rtc/rtc.c $(DIR)rtc/rtc.h
	$(CC) $(OPT) -c $(DIR)rtc/rtc.c
lcd.o: $(DIR)lcd/lcd.c $(DIR)lcd/nokia5110.h $(DIR)lcd/nokia5110_chars.h
	$(CC) $(OPT) -c $(DIR)lcd/lcd.c

prog.hex: prog
	avr-objcopy -O ihex prog prog.hex
upload: prog.hex
	avrdude -c $(INTERFACE) -p $(MMCU_AVRDUDE) -P $(DEVICE) -b $(SPEED)  -U flash:w:prog.hex

clean:
	rm main.o lcd.o rtc.o
