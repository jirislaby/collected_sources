LD=ld
ASFLAGS=--32

all: a.bin

a.bin: a
	objcopy -O binary $< $@

a: x.lds a.o
	$(LD) -melf_i386  -T $< -o $@ a.o
