CC=gcc
CXX=g++
CFLAGS=-g -Wall -O2
INCLUDE=-I/public/VRECKO/unstable/linux_test/include
LIBS=-L/public/VRECKO/unstable/linux_test/lib -lvrecko -losg -lProducer -lesg -lOpenThreads -losgDB -lpng -lxerces-c -lvecmath -lesg_osg -lphantom

all:test

clean:
	rm -f core* *.o *.a test

test:lmain.cpp linux.h windows.h FF.h
	$(CXX) $(CFLAGS) $(INCLUDE) $(LIBS) -otest lmain.cpp
