VRECKO=/public/VRECKO/unstable/linux_test

CC=gcc
CXX=g++
CXXFLAGS=-g -Wall -O2 -I$(VRECKO)/include -MMD -MF .$@.d -fomit-frame-pointer
LDFLAGS=-L$(VRECKO)/lib -lvrecko -losg -lProducer -lesg -lOpenThreads -losgDB -lpng -lxerces-c -lvecmath -lesg_osg -lphantom

DEPS=$(wildcard .*.d)

all: vrecko_ff.so

clean:
	rm -rf core* *.o $(DEPS)

mrproper: clean
	rm -f *.so *.a

vrecko_ff.so: platform.o

.PHONY: all clean mrproper

-include $(DEPS)
