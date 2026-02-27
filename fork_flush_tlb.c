#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/io.h>
#include <sys/types.h>
#include <sys/wait.h>

#define ITER 1000
#define FORKERS 15
#define OPENS 10

static void flush_tlb()
{
	for (unsigned i = 0; i < OPENS; ++i)
		if (!open("/dev/bubak", O_RDWR) || errno != EIO)
			errx(1, "open");
	exit(0);
}

static void run_forkers()
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
			//printf("\tforker%d %d\n", a, forkers[a]);
			break;
		}
	}

	for (a = 0; a < FORKERS; a++) {
		waitpid(forkers[a], NULL, 0);
		//printf("\tforker%d (%d) done\n", a, forkers[a]);
	}
}

int main()
{
	unsigned a;
	int ret;

	ret = ioperm(10, 20, 0);
	if (ret < 0)
		err(1, "ioperm");

	for (a = 0; a < ITER; a++)
		run_forkers();

	return 0;
}
