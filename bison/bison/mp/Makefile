CC=g++
STDP=-Wall -g
OBJS=mpy.o main.o mpyy.o 
LIBS=-lfl -lm
LEX=flex
YACC=bison

all:mp

mp:$(OBJS)
	$(CC) $(STDP) -omp $(OBJS) $(LIBS)

main.o:main.cpp
	$(CC) -c $(STDP) -omain.o main.cpp

mpyy.o:mp-yy.cpp
	$(CC) -c $(STDP) -ompyy.o mp-yy.cpp

mp-yy.cpp:mp.yy
	$(LEX) -omp-yy.cpp mp.yy

mpy.o:mp-y.cpp
	$(CC) -c $(STDP) -ompy.o mp-y.cpp

mp-y.cpp:mp.y
	$(YACC) -v -d -omp-y.cpp mp.y

clean:
	rm -rf *.c *.o *.output mp-y*

mrproper: 
	rm -rf *.c *.o *.output mp-y* mp
