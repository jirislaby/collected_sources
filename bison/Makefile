CC=g++
STDP=-Wall -g
OBJS=mpy.o mpyy.o 
LIBS=-lfl
LEX=flex
YACC=bison

all:mp

mp:$(OBJS)
	$(CC) $(STDP) -omp $(OBJS) $(LIBS)

mpyy.o:mp-yy.cpp main.h
	$(CC) -c $(STDP) -ompyy.o mp-yy.cpp

mp-yy.cpp:mp.yy
	$(LEX) -I -omp-yy.cpp mp.yy

mpy.o:mp-y.cpp main.h
	$(CC) -c $(STDP) -ompy.o mp-y.cpp

mp-y.cpp:mp.y
	$(YACC) -v -d -omp-y.cpp mp.y

clean:
	rm -rf *.c *.o *.output mp-y*
	cd preproc;make clean;cd ..

mrproper: 
	rm -rf *.c *.o *.output mp-y* mp
	cd preproc;make mrproper;cd ..
