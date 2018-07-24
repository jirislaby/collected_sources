KSRC=/lib/modules/$(shell uname -r)/source
AWK=awk
CFLAGS=-Wall -Og -g -I$(KSRC)/arch/x86/include/
LIBS=-lm

all: main

main: main.o insn.o inat.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

insn.o: CFLAGS+='-Dunlikely(x)=x'

insn.c inat.c:
	cp $(KSRC)/arch/x86/lib/$@ .

inat.c: inat-tables.c

inat-tables.c: $(KSRC)/arch/x86/tools/gen-insn-attr-x86.awk $(KSRC)/arch/x86/lib/x86-opcode-map.txt
	$(AWK) -f $(KSRC)/arch/x86/tools/gen-insn-attr-x86.awk $(KSRC)/arch/x86/lib/x86-opcode-map.txt >$@

clean:
	rm -f *.o inat-tables.c insn.c inat.c
