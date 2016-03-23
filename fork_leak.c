#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/io.h>
#include <sys/types.h>
#include <sys/wait.h>

#define ITER 1000
#define FORKERS 15
#define THREADS (6000/FORKERS) // 1850 is proc max

static void fork_100_wait()
{
	unsigned a, to_wait = 0;
	pid_t pid;

	printf("\t%d forking %d\n", THREADS, getpid());

	for (a = 0; a < THREADS; a++) {
		switch ((pid = fork())) {
		case 0:
			usleep(1000);
			exit(0);
			break;
		case -1:
			//warn("NODIE fork");
			break;
		default:
			to_wait++;
			break;
		}
	}

	printf("\t%d forked from %d, waiting for %d\n", THREADS, getpid(),
			to_wait);

	for (a = 0; a < to_wait; a++)
		wait(NULL);

	printf("\t%d waited from %d\n", THREADS, getpid());
}

static void run_forkers()
{
	pid_t forkers[FORKERS];
	unsigned a;

	for (a = 0; a < FORKERS; a++) {
		switch ((forkers[a] = fork())) {
		case 0:
			fork_100_wait();
			exit(0);
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
