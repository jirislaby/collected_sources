CC=gcc
CFLAGS+= -I /usr/include/GL/ -Wall -O2 -g -fomit-frame-pointer \
	-Wstrict-prototypes
LDLIBS+= -lglut -lGL -lGLU

all: projekt

projekt.c: data.h

clean:
	rm -f projekt *.o

.PHONY: all clean
