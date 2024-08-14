CC=gcc
OBJCOPY=objcopy

CRT0=/usr/lib64/crt0-efi-x86_64.o
LD_SCRIPT=/usr/lib64/elf_x86_64_efi.lds

INCLUDE_DIRS=/usr/include/efi /usr/include/efi/x86_64
CFLAGS=-Wall -g -O2 -DGNU_EFI_USE_MS_ABI -DEFI_FUNCTION_WRAPPER -fmessage-length=0 -mno-red-zone -fno-stack-protector -fno-strict-aliasing -fpic -fshort-wchar $(addprefix -I,$(INCLUDE_DIRS))

LDFLAGS=-nostdlib -znocombreloc
LDLIBS=-L/usr/lib64 -lefi -lgnuefi
#-L/usr/lib64/gcc/x86_64-suse-linux/10/ -lgcc

OBJS=acpi.o main.o pic.o

COPY_SECTIONS=.text .sdata .data .dynamic .dynsym .rel .rela .rel.* .rela.* \
	      .reloc

all: main.efi

%.efi: %.so
	$(OBJCOPY) $(addprefix -j ,$(COPY_SECTIONS)) --target efi-app-x86_64 $^ $@

%.so: $(OBJS)
	$(LD) $(LDFLAGS) -shared -Bsymbolic -T$(LD_SCRIPT) -o $@ $(CRT0) $^ $(LDLIBS)
