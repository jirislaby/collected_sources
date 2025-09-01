#KDIR=/l/latest/a/kgr_van
KDIR=/l/latest/my/
KBUILD=$(MAKE) -C $(KDIR) M=$(PWD)
NAME=ftr_test

obj-m := $(NAME).o

default:
	$(KBUILD) modules
	$(KDIR)/source/scripts/sign-file sha256 $(KDIR)/signing_key.priv $(KDIR)/signing_key.x509 $(NAME).ko

clean:
	$(KBUILD) clean

.PHONY: default clean
