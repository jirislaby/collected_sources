IS32=-m32
#STATIC=-static
CFLAGS=-Wall -O2 -fomit-frame-pointer -g $(IS32) -nostdinc \
	-I/home/l/latest/my_64/usr/include \
	-I/usr/include/ \
	-I/usr/lib64/gcc/x86_64-suse-linux/4.5/include/
CC=gcc
LDFLAGS=$(STATIC) $(IS32)

all: lim
	strip lim

lim: lim.o

clean:
	rm -f *.o lim

.PHONY: all clean
