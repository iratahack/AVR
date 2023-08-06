PROJ=blink_led
CC=avr-gcc
STRIP=avr-strip
OBJDUMP=avr-objdump
OBJCOPY=avr-objcopy
COPTS=-Os
MCU=atmega328p
CLK=16000000UL

CSRC:=$(wildcard *.c)
OBJS:=$(CSRC:.c=.o)

.PHONY: all clean download dis

all: $(PROJ)

clean:
	rm -f $(PROJ) *.o *.hex

%.o: %.c
	$(CC) $(COPTS) -DF_CPU=$(CLK) -mmcu=$(MCU) -c -o $@ $<

$(PROJ): $(OBJS)
	$(CC) -mmcu=$(MCU) $(OBJS) -o $@
	$(STRIP) $@
	$(OBJDUMP) -h $@

%.hex: %
	$(OBJCOPY) -O srec $< $@

download: $(PROJ).hex
	sudo avrdude -v -c arduino -p $(MCU) -P /dev/ttyUSB0 -b 115200 -U flash:w:$<

dis: $(PROJ)
	avr-objdump -S $(PROJ) | less
