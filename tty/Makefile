CFLAGS=-Wall -Wextra -Og -ggdb -MD
PROGS=csi_P kdgkb ld ld1 ld2 n_gsm pty pty_multiwrite serial tiocm tiocsetd tty1
PROGS+=tty_double
PROGS+=tty_double_r tty-loop-hangup tty-loop-open tty-loop-rw
PROGS+=tty-loop-tiocsctty tty-loop-tiocsetd tty_name tty_printk tty_unimap
PROGS+=vcs_test

all: $(PROGS)

pty_multiwrite: LDFLAGS+=-pthread
tty_unimap: LDFLAGS+=-pthread

%: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

-include *.d

clean:
	rm -f *.o *.d $(PROGS)
