#include <err.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#if 0
#include <asm/termios.h>
#else

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
#endif

static volatile bool killed;

static bool send_char(int tty, char x)
{
	if (write(tty, &x, 1) <= 0) {
		warn("cannot send %hhx", x);
		return false;
	}

	return true;
}

static bool set_termios2(int tty, const struct termios2 *t)
{
	if (ioctl(tty, TCSETSW2, t)) {
		warn("cannot set termios");
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

static void send_dmx512(int tty, const struct termios2 *t_57,
		const struct termios2 *t_250)
{
	while (!killed) {
		if (!set_termios2(tty, t_57))
			break;
		/* send BREAK */
		if (!send_char(tty, 0x00))
			break;
		if (!set_termios2(tty, t_250))
			break;
#if 0
		/* send MAB */
		if (!send_char(tty, 0xff))
			break;
		/* send start code */
		if (!send_char(tty, 0))
			break;
#endif
		if (!send_seq(tty))
			break;
	}
}

static void flush(int tty)
{
	if (ioctl(tty, TCFLSH, TCIOFLUSH))
		err(EXIT_FAILURE, "cannot flush");
}

static void setup(int tty, struct termios2 *orig, struct termios2 *t_57,
		struct termios2 *t_250)
{
	if (ioctl(tty, TCGETS2, orig))
		err(EXIT_FAILURE, "cannot call TCGETS2");

	memcpy(t_57, orig, sizeof(*orig));
	memcpy(t_250, orig, sizeof(*orig));

	t_57->c_cflag &= ~CBAUD;
	t_57->c_cflag |= BOTHER | CLOCAL | CS7;
	t_57->c_ispeed = 57000;
	t_57->c_ospeed = 57000;

	t_250->c_cflag &= ~CBAUD;
	t_250->c_cflag |= BOTHER | CLOCAL | CS8;
	t_250->c_ispeed = 250000;
	t_250->c_ospeed = 250000;
}

static void sig(int s)
{
	puts("killed");
	killed = true;
}

static void setup_signals(void)
{
	struct sigaction act = {
		.sa_handler = sig,
	};
	int signals[] = { SIGTERM, SIGINT };
	int a;

	for (a = 0; a < sizeof(signals)/sizeof(*signals); a++)
		if (sigaction(signals[a], &act, NULL) < 0)
			err(EXIT_FAILURE, "cannot setup signal %d", signals[a]);
}

int main(int argc, char **argv)
{
	struct termios2 orig, t_57, t_250;
	int tty;

	if (argc < 2)
		return EXIT_FAILURE;

	setup_signals();

	tty = open(argv[1], O_RDWR | O_NONBLOCK);
	if (tty < 0)
		err(EXIT_FAILURE, "cannot open tty %s", argv[1]);

	flush(tty);
	setup(tty, &orig, &t_57, &t_250);

	send_dmx512(tty, &t_57, &t_250);

	puts("cleaning up");

	if (ioctl(tty, TCSETSW2, &orig))
		err(EXIT_FAILURE, "cannot set termios");

	close(tty);

	return 0;
}
