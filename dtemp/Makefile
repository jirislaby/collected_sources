CC=gcc
CFLAGS=-Wall -Wstrict-prototypes -MMD -MF .$@.d -O2 -fomit-frame-pointer -g
DEPS=$(wildcard .*.d)

all: dtemp

dtemp: dtemp.o

clean:
	@rm -f *.o dtemp $(DEPS)

.PHONY: all clean

-include $(DEPS)
