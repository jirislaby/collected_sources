MCU_TARGET=atmega128
F_CPU=14745600
CC=avr-gcc
CFLAGS=-Wall -Wstrict-prototypes -MMD -MF .$@.d -Os -fomit-frame-pointer -g \
	-mmcu=$(MCU_TARGET) -DF_CPU=$(F_CPU)
DEPS=$(wildcard .*.d)

all: lcd

lcd: lcd.o main.o diasel.o

clean:
	@rm -f *.o lcd $(DEPS)

.PHONY: all clean

-include $(DEPS)
