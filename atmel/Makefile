PROJ=ac
MCU=attiny2313
OBJCOPY=avr-objcopy
CC=avr-gcc
CFLAGS=-mmcu=$(MCU) -Wall -g -mcall-prologues -Os
LDFLAGS=-mmcu=$(MCU) -Wl,-Map,$(PROJ).map
DUDE=avrdude
DUDETYPE=t2313

OBJS=$(PROJ).o

all: $(PROJ).hex $(PROJ).bin

$(PROJ).hex: $(PROJ)
	$(OBJCOPY) -O ihex -R eeprom $^ $@

$(PROJ).bin: $(PROJ)
	$(OBJCOPY) -O binary -j .text $^ $@

$(PROJ): $(OBJS)

clean:
	rm -f $(PROJ) $(PROJ).map $(PROJ).hex $(PROJ).bin *.o

prog: $(PROJ).hex
	$(DUDE) -c dapa -p $(DUDETYPE) -U flash:w:$^ -F

.PHONY: all prog clean
