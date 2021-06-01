#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

static void rand_sched()
{
	unsigned int rnd = rand();
	unsigned int ms =		rnd & 0x000003ff; // 1023
	unsigned int sleep_sched =	rnd & 0x00001000;

	if (!(rnd % 4)) {
		if (sleep_sched)
			usleep(1000 + 1000 * ms);
		else if (sched_yield())
			err(2, "sched_yield");
	}
}

static void rd(unsigned int thr, const char *tty)
{
	char buf[256];
	int fd, a;

	fd = open(tty, O_RDWR | O_NONBLOCK);
	if (fd < 0)
		err(2, "open");

	for (a = 0; a < 10; a++) {
		ssize_t rd = read(fd, buf, sizeof(buf));

		if (rd < 0 && errno != EAGAIN)
			err(2, "%u: read", thr);

		if (rd >= 0) {
			if (rd > 0 && buf[rd - 1] == '\n')
				buf[rd - 1] = 0;
			printf("%u: %3zd %*s\n", thr, rd, (int)rd, buf);
		}

		rand_sched();
	}

	rand_sched();

	close(fd);

	rand_sched();
}

static void work(unsigned int thr, const char *tty)
{
	while (1)
		rd(thr, tty);
}

static void setup_serial(const char *tty)
{
	struct termios tios;
	int fd = open(tty, O_RDWR | O_NONBLOCK);

	if (fd < 0)
		err(1, "open");

	if (tcflush(fd, TCIOFLUSH) < 0)
		err(1, "tcflush");

	if (tcgetattr(fd, &tios) < 0)
		err(1, "tcgetattr");

	if (cfsetspeed(&tios, 115200) < 0)
		err(1, "cfsetspeed");

	tios.c_lflag &= ~(ECHO|ECHONL);

	if (tcsetattr(fd, TCSANOW, &tios) < 0)
		err(1, "tcsetattr");

	close(fd);
}

static const unsigned char FORKS = 3;

int main(int argc, char **argv)
{
	unsigned int a;

	setup_serial(argv[1]);

	for (a = 0; a < FORKS; a++) {
		switch (fork()) {
		case -1:
			err(1, "fork");
		case 0:
			work(a, argv[1]);
		default:
			break;
		}
	}

	for (a = 0; a < FORKS; a++)
		wait(NULL);

	return 0;
}
