#CFLAGS_PROFILE=-pg -mfentry
CFLAGS=-Wall -O2 -g $(CFLAGS_PROFILE) $(shell pkg-config --cflags pciaccess)
CFLAGS_LTO=-flto
LDLIBS=$(shell pkg-config --libs pciaccess)

all: lto no_lto

lto: main.lto.o lib.lto.o
	$(CC) $(CFLAGS) $(CFLAGS_LTO) -o $@ $(LDLIBS) $^

no_lto: main.o lib.o
	$(CC) $(CFLAGS) -o $@ $(LDLIBS) $^

%.lto.o: %.c
	$(CC) $(CFLAGS) $(CFLAGS_LTO) -c -o $@ $<

main.c: lib.h

lib.c: lib.h
