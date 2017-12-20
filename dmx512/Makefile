CFLAGS=-Wall -O2 -g -pie
LDFLAGS=-Wall -O2 -g
CC=gcc
LD=gcc

all: dmx512

dmx512: main.o
	$(LD) $(LDFLAGS) -o $@ $<

clean:
	rm -f *.o

.PHONY: clean all
