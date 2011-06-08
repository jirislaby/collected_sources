CC=gcc
CFLAGS=-Wall -g -O2
OBJS=lex.o main.o
LDFLAGS=-lfl -g -O2
LEX=flex
YACC=bison

all: asm_filter

asm_filter: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

lex.c: lex.ll

%.c: %.ll
	$(LEX) --header-file=$(patsubst %.c,%.h,$@) -o $@ $^

clean:
	rm -f *.c *.o *.output
