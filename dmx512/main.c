#include <err.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

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

int main(int argc, char **argv)
{
	int tty;

	if (argc < 2)
		return EXIT_FAILURE;

	tty = open(argv[1], O_RDWR);
	if (tty < 0)
		err(EXIT_FAILURE, "cannot open tty %s", argv[1]);

	send_dmx512(tty);

	close(tty);

	return 0;
}
