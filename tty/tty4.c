#include <err.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

static void do_work(const char *tty)
{
	int fd;

	if (signal(SIGHUP, SIG_IGN) == SIG_ERR)
		err(1, "signal(SIGHUP)");

	setsid();

	while (1) {
		fd = open(tty, O_RDWR|O_NOCTTY);
		if (fd < 0) {
			warn("open");
			continue;
		}
		if (ioctl(fd, TIOCSCTTY, 1))
			warn("ioctl");
		close(fd);
	}
	exit(0);
}

int main(int argc, char **argv)
{
	pid_t pid;

	switch (pid = fork()) {
	case 0:
		do_work(argv[1]);
		break;
	case -1:
		err(1, "fork");
		break;
	default:
	{
		int stat;
		waitpid(pid, &stat, 0);
		if (stat) {
			fprintf(stderr, "exited with: %d sig=%d signr=%u\n",
					WEXITSTATUS(stat), WIFSIGNALED(stat),
					WTERMSIG(stat));
		}
		break;
	}
	}

	return 0;
}
