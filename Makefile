CC=gcc
CFLAGS=-Wall -Og -ggdb
TARGETS=fcntl fork_leak ipv6 malloc_pages open_loop personality pgid_sid remote_tuner root sig snd tun usb_killer vhci

all: $(TARGETS)

clean:
	rm -f $(TARGETS)

.PHONY: all clean
