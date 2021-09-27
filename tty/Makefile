CFLAGS=-Wall -Og -ggdb -MD

all: csi_P kdgkb ld ld1 ld2 n_gsm pty serial tiocm tiocsetd tty1 tty4 tty_double tty_double_r tty-loop-hangup tty-loop-open tty-loop-read tty-loop-tiocsetd tty_printk vcs_test

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

-include *.d
