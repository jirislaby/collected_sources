CFLAGS= -Wall -fomit-frame-pointer -O2 -Wstrict-prototypes -MMD -MF .$@.d
CC=gcc
LDFLAGS= -lrt
DEPS=$(wildcard .*.d)

all: ir

ir: ir.o

clean:
	rm -f *.o

.PHONY: all clean

-include $(DEPS)
