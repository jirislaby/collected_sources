CFLAGS=-Wall -O2 -g -pie
LD=gcc

all: dmx512

dmx512: main.o
	$(LD) $(LDFLAGS) -o $@ $<

clean:
	rm -f *.o

.PHONY: clean all
