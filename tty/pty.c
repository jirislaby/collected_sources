#define _XOPEN_SOURCE
#define _GNU_SOURCE
#include <err.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <termios.h>

void send(int ptmx, int fd)
{
	char buf[4096];
	int counter = 0;

	while (1) {
		int op = rand() % 4;
		if (ioctl(fd, TCXONC, op) < 0)
			err(1, "strcmp");
		usleep(100 * (1 + rand() % 20));
		if (!(counter++ % 1000)) {
			size_t rd = read(ptmx, buf, sizeof(buf));
			printf("%d: %5zd %2x op=%d\n", getpid(), rd, rd > 0 ? buf[0] : 0, op);
			buf[0] = 0;
		}
	}
}

#define THREADS 10

void fork_send(int ptmx, int fd)
{
	int a;
	for (a = 0; a < THREADS - 1; a++) {
		if (!fork())
			break;
	}
	srand(getpid());
	send(ptmx, fd);
}

int main(int argc, char **argv)
{
	char path[PATH_MAX];
	int fd, ptmx, rnd;
	while (1) {
		ptmx = open("/dev/ptmx", O_RDWR | O_NONBLOCK);
		if (ptmx < 0)
			err(1, "open ptmx");

		if (grantpt(ptmx))
			err(1, "grantpt");

		if (unlockpt(ptmx))
			err(1, "unlockpt");

		if (ptsname_r(ptmx, path, sizeof(path)))
			err(1, "ptsname");

		fd = open(path, O_RDWR);
		if (fd < 0)
			err(1, "open slave");

		if (write(fd, "ahoj", 4) != 4)
			err(1, "write");
		if (read(ptmx, path, 4) != 4)
			err(1, "read");
		if (strncmp("ahoj", path, 4))
			errx(1, "strcmp");

		fork_send(ptmx, fd);

		rnd = rand();
		usleep(150 * (1 + rnd % 80));
		if (rnd & 0x40000000) {
			close(fd);
			close(ptmx);
		} else {
			close(ptmx);
			close(fd);
		}
	}
	return 0;
}
