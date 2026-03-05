#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/io.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>

#if 1
#define FORKERS 15
#define OPENS 100
#else
#define FORKERS 1
#define OPENS 1
#endif

static void flush_tlb()
{
	int fd = open("/dev/bubak", O_RDWR);
	if (fd < 0)
		err(1, "open");
	for (unsigned i = 0; i < OPENS; ++i)
		if (!ioctl(fd, 0, 0) || errno != EIO)
			errx(1, "ioctl");
	exit(0);
}

int main()
{
	pid_t forkers[FORKERS];
	unsigned a;

	for (a = 0; a < FORKERS; a++) {
		switch ((forkers[a] = fork())) {
		case 0:
			flush_tlb();
			break;
		case -1:
			err(1, "DIE fork of %d'th forker", a);
			break;
		default:
			break;
		}
	}

	flush_tlb();

	for (a = 0; a < FORKERS; a++)
		waitpid(forkers[a], NULL, 0);

	return 0;
}
