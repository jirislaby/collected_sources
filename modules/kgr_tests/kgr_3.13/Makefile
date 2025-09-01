KDIR=/l/latest/tmp/linux1
KBUILD=$(MAKE) -C $(KDIR) M=$(PWD)

obj-m := kgr_patcher.o

default:
	$(KBUILD) modules

clean:
	$(KBUILD) clean

us: us.c

.PHONY: default clean
