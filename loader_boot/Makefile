TARGET=head head.bin
AS=as
ASFLAGS=-g --32
LD=ld
OBJCOPY=objcopy
LOBJS=head.o
OBJS=$(LOBJS) $(COMMON_OBJS)

all: $(TARGET)

%.o: %.s
	$(AS) $(ASFLAGS) -o $@ $<

head: os.lds $(OBJS)
	$(LD) $(LDFLAGS) -T os.lds -melf_i386 -o $@ $(OBJS)

%.bin: %
	$(OBJCOPY) -O binary $< $@
