CC=gcc
CFLAGS=-Wall -Og -ggdb
TARGETS=af_unix cond_wait fadvise fcntl fork_leak gai ipv6 malloc_pages \
	mmap msgrcv open_loop personality \
	pgid_sid poll_sel pthread_once remote_tuner recv_udp \
	root sig snd tun usb_killer vhci

cond_wait: CFLAGS+=-pthread
pthread_once: CFLAGS+=-pthread

all: $(TARGETS)

clean:
	rm -f $(TARGETS)

.PHONY: all clean
