KDIR=/home/xslaby/ku/my
KBUILD=$(MAKE) -C $(KDIR) M=$(PWD)
MOD=livepatch-sample

obj-m := $(MOD).o

default:
	$(KBUILD) modules
	$(KDIR)/source/scripts/sign-file sha256 $(KDIR)/signing_key.priv $(KDIR)/signing_key.x509 $(MOD).ko

clean:
	$(KBUILD) clean

us: us.c

.PHONY: default clean
