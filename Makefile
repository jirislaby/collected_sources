CC=gcc
CFLAGS=-Wall -Og -ggdb
TARGETS=cond_wait fcntl fork_leak ipv6 malloc_pages mmap open_loop personality \
	pgid_sid remote_tuner root sig snd tun usb_killer vhci

cond_wait: CFLAGS+=-pthread

all: $(TARGETS)

clean:
	rm -f $(TARGETS)

.PHONY: all clean
