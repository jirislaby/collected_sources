#include <err.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

static void do_work(const char *tty)
{
	if (signal(SIGHUP, SIG_IGN) == SIG_ERR)
		exit(1);
	setsid();
	while (1) {
		int fd = open(tty, O_RDWR|O_NOCTTY);
		if (fd < 0) {
			warn("open");
			continue;
		}
		if (ioctl(fd, TIOCSCTTY, 1))
			warn("ioctl");
		else if (vhangup()) warn("vhangup");
		close(fd);
	}
	exit(0);
}


int main(int argc, char **argv)
{
	switch (fork()) {
	case -1:
		err(1, "fork");
	case 0:
		do_work(argv[1]);
		break;
	default:
		break;
	}

	return 0;
}
