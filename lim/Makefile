#IS32=-m32
#STATIC=-static
CFLAGS=-Wall -O2 -fomit-frame-pointer -g $(IS32)
CC=gcc
LDFLAGS=$(STATIC) $(IS32)

all: lim
	strip lim

lim: lim.o

clean:
	rm -f *.o lim

.PHONY: all clean
