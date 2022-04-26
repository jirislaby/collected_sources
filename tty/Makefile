CFLAGS=-Wall -Og -ggdb -MD
PROGS=csi_P kdgkb ld ld1 ld2 n_gsm pty serial tiocm tiocsetd tty1 tty_double
PROGS+=tty_double_r tty-loop-hangup tty-loop-open tty-loop-rw
PROGS+=tty-loop-tiocsctty tty-loop-tiocsetd tty_name tty_printk vcs_test

all: $(PROGS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

-include *.d

clean:
	rm -f *.o *.d $(PROGS)
