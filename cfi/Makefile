CC=gcc
CFLAGS=-Wall -g -O2 -fomit-frame-pointer
LDFLAGS=-rdynamic

all: cfi

cfi: a.o c.o
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	@rm -f *.o

.PHONY: clean all
