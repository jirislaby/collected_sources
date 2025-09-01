KDIR=/l/latest/my/
KBUILD=$(MAKE) -C $(KDIR) M=$(PWD)

obj-m := kgr_hello.o

default:
	$(KBUILD) modules

clean:
	$(KBUILD) clean

.PHONY: default clean
