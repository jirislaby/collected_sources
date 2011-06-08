CFLAGS=-O2 -g -Wall -fomit-frame-pointer -Wstrict-prototypes
LDFLAGS=-lpthread
CC=gcc

all: ser cl kl

ser: ser.o

cl: cl.o

kl: kl.o

clean:
	rm -f kl.o kl cl.o cl ser.o ser

.PHONY: all
