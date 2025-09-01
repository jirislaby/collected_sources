#KDIR=/l/latest/a/kgr_van
#KDIR=/l/latest/my/
KDIR=/lib/modules/$(shell uname -r)/build
KBUILD=$(MAKE) -C $(KDIR) M=$(PWD)
MOD=aaa

obj-m := $(MOD).o

default:
	$(KBUILD) modules
#	$(KDIR)/source/scripts/sign-file sha256 $(KDIR)/signing_key.priv $(KDIR)/signing_key.x509 $(MOD).ko

clean:
	$(KBUILD) clean

us: CFLAGS=-Wall -O2 -g

us: us.c

.PHONY: default clean
