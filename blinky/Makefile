PROJ:=blink_led
CC:=avr-gcc
COPTS:=-Os
MCU:=atmega328p
CSRC=$(wildcard *.c)
OBJS=$(CSRC:.c=.o)
CLK=16000000UL

.PHONY: all clean download dis

all: $(PROJ)

clean:
	rm -f $(PROJ) *.o *.hex

%.o: %.c
	$(CC) $(COPTS) -DF_CPU=$(CLK) -mmcu=$(MCU) -c -o $@ $<

$(PROJ): $(OBJS)
	$(CC) -mmcu=$(MCU) $(OBJS) -o $@

%.hex: %
	avr-objcopy -O ihex -R .eeprom $< $@

download: $(PROJ).hex
	sudo avrdude -F -V -c arduino -p $(MCU) -P /dev/ttyUSB0 -b 115200 -U flash:w:$<

dis: $(PROJ)
	avr-objdump -S $(PROJ) | less
