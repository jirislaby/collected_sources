#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

static void steal_back(void)
{
	int s0 = open("/dev/ttyS0", O_RDWR|O_NOCTTY);
	if (s0 < 0)
		err(1, "steal open");
	if (ioctl(s0, TIOCSCTTY, 1))
		warn("ioctl");
	close(s0);
}

static void loop(const char *tty)
{
//	char buf[128];
	int fd;

	while (1) {
		fd = open(tty, O_RDWR);
		if (fd < 0) {
			if (errno == ENXIO)
				steal_back();
/*			else
				err(1, "open: err=%d", errno);*/
			continue;
		}
/*		if (read(fd, buf, sizeof(buf)) < 0)
			warn("read");*/
		close(fd);
	}
}

static void do_work(const char *tty)
{
	if (signal(SIGHUP, SIG_IGN) == SIG_ERR)
		err(1, "signal(SIGHUP)");

	setsid();

	loop(tty);
}

int main(int /*argc*/, char **argv)
{
	pid_t pid;

	loop(argv[1]);
	return 0;

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
