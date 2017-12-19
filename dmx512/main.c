#include <err.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

//#include <asm/termios.h>

#define    BOTHER 0010000

struct termios2 {
	tcflag_t c_iflag;               /* input mode flags */
	tcflag_t c_oflag;               /* output mode flags */
	tcflag_t c_cflag;               /* control mode flags */
	tcflag_t c_lflag;               /* local mode flags */
	cc_t c_line;                    /* line discipline */
	cc_t c_cc[19];			/* control characters */
	speed_t c_ispeed;               /* input speed */
	speed_t c_ospeed;               /* output speed */
};

static bool send_char(int tty, char x)
{
	if (write(tty, &x, 1) <= 0) {
		warn("cannot send %hhx", x);
		return false;
	}

	return true;
}

static bool send_seq(int tty)
{
	char seq[64] = { [0 ... 63] = 0xff };

	if (write(tty, seq, sizeof(seq)) <= 0) {
		warn("cannot send sequence");
		return false;
	}

	return true;
}

static void send_dmx512(int tty)
{
	while (1) {
		if (tcsendbreak(tty, 0)) {
			warn("cannot send break");
			break;
		}
		/* send MAB */
		if (!send_char(tty, 0xff)) /* TODO wrong */
			break;
		/* send start code */
		if (!send_char(tty, 0))
			break;
		if (!send_seq(tty))
			break;
	}
}

static void setup(int tty)
{
	struct termios2 t;

	if (ioctl(tty, TCGETS2, &t))
		err(EXIT_FAILURE, "cannot call TCGETS2");

	t.c_cflag |= CLOCAL;
	t.c_cflag &= ~CBAUD;
	t.c_cflag = BOTHER;
	t.c_ospeed = 250000;

	if (ioctl(tty, TCSETS2, &t))
		err(EXIT_FAILURE, "cannot call TCSETS2");
}
int main(int argc, char **argv)
{
	int tty;

	if (argc < 2)
		return EXIT_FAILURE;

	tty = open(argv[1], O_RDWR | O_NONBLOCK);
	if (tty < 0)
		err(EXIT_FAILURE, "cannot open tty %s", argv[1]);

	tcdrain(tty);
	setup(tty);

	send_dmx512(tty);

	close(tty);

	return 0;
}
