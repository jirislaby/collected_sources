CC=gcc
CFLAGS=-O2 -Wall -g
MAKEASM=$(CC) $(CFLAGS) -S #-fverbose-asm

all: pg.s src.s fentry.s src

src: src.o kiss.o
	$(CC) $(CFLAGS) -pg -mfentry -o $@ $^

pg.s: src.c
	$(MAKEASM) -pg -o $@ $<

fentry.s: src.c
	$(MAKEASM) -pg -mfentry -o $@ $<

%.s: %.c
	$(MAKEASM) -o $@ $<
