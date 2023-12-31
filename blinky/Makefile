PROJ=blink_led
CC=avr-gcc
STRIP=avr-strip
SIZE=avr-size
OBJDUMP=avr-objdump
OBJCOPY=avr-objcopy
MCU=atmega328p
CLK=16000000UL
CFLAGS=-O3 -DF_CPU=$(CLK) -mmcu=$(MCU)

CSRC:=$(wildcard *.c)
OBJS:=$(CSRC:.c=.o)

.PHONY: all clean download dis

all: $(PROJ)

clean:
	rm -f $(PROJ) *.o *.hex

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(PROJ): $(OBJS)
	$(CC) -mmcu=$(MCU) $? -o $@
	$(SIZE) --format=avr --mcu=atmega328p $@

%.hex: %
	$(OBJCOPY) -O ihex $< $@

download: $(PROJ).hex
	sudo avrdude -v -c arduino -p $(MCU) -P /dev/ttyUSB0 -b 115200 -U flash:w:$<

dis: $(PROJ)
	avr-objdump -S $(PROJ) | less
