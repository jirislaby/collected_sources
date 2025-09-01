#KDIR=/l/latest/a/kgr_van
KDIR=/l/latest/my/
KBUILD=$(MAKE) -C $(KDIR) M=$(PWD)

obj-m := kgraft_patcher1.o

default:
	$(KBUILD) modules
	$(KDIR)/source/scripts/sign-file sha256 $(KDIR)/signing_key.priv $(KDIR)/signing_key.x509 kgraft_patcher1.ko

clean:
	$(KBUILD) clean

us: us.c

.PHONY: default clean
