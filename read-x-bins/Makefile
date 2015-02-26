CFLAGS=-Wall -O2 -g -fPIC
LD=gcc
LDFLAGS=-shared -fPIC

all: read.so

%.so: %.o
	$(LD) $(LDFLAGS) -o $@ $<
