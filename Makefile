CC=gcc
CFLAGS=-Wall -Og -ggdb
TARGETS=fork_leak ipv6 malloc_pages open_loop remote_tuner root tun vhci

all: $(TARGETS)

clean:
	rm -f $(TARGETS)

.PHONY: all clean
