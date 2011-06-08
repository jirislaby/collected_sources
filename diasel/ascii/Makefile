CC=gcc
CFLAGS=-Wall -Wstrict-prototypes -MMD -MF .$@.d -O2 -fomit-frame-pointer -g
DEPS=$(wildcard .*.d)

all: ascii

ascii: ascii.o

clean:
	@rm -f *.o ascii $(DEPS)

.PHONY: all clean

-include $(DEPS)
