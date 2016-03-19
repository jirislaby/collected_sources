CC=gcc
CFLAGS=-Wall -Og -ggdb

all: ipv6 remote_tuner root vhci

clean:
	@rm -f ipv6 remote_tuner root
