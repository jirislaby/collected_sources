CFLAGS=-Wall -Wstrict-prototypes -fomit-frame-pointer -Werror -g
CC=gcc
LD=gcc

all: mpd-dbus

mpd-dbus: main.o
	$(LD) $(LDFLAGS) -o $@ $^ 

clean:
	rm -f *.o mpd-dbus

.PHONY: clean all
