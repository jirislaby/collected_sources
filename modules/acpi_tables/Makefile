KDIR=/lib/modules/$(shell uname -r)/build
KBUILD=$(MAKE) -C $(KDIR) M=$(PWD)

obj-m := acpi_tables.o

default:
	$(KBUILD) modules

%.s: %.c
	$(KBUILD) $@

%.s: %.S
	$(KBUILD) $@

clean:
	$(KBUILD) clean

.PHONY: default clean
