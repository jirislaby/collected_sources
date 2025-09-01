KDIR=/home/xslaby/ku/my/
#KDIR=/lib/modules/$(shell uname -r)/build
KBUILD=$(MAKE) -C $(KDIR) M=$(PWD)
MOD=mxser

obj-m := $(MOD).o serial_core.o

default:
	$(KBUILD) $(MOD).o serial_core.o
#	$(KDIR)/source/scripts/sign-file sha256 $(KDIR)/signing_key.priv $(KDIR)/signing_key.x509 $(MOD).ko

clean:
	$(KBUILD) clean

.PHONY: default clean
