CC=gcc
CFLAGS=-Wall -Og -ggdb
TARGETS=ipv6 remote_tuner root vhci

all: $(TARGETS)

clean:
	rm -f $(TARGETS)

.PHONY: all clean
