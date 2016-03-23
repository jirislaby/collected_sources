#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/io.h>
#include <sys/types.h>
#include <sys/wait.h>

#define ITER 100
#define FORKERS 15
#define THREADS (1700/FORKERS) // 1850 is proc max

static void fork_100_wait()
{
	unsigned a;
	pid_t pid;

	for (a = 0; a < THREADS; a++) {
		switch ((pid = fork())) {
		case 0:
			usleep(1000);
			exit(0);
			break;
		case -1:
			err(1, "fork");
			break;
		default:
			break;
		}
	}

	printf("100 forked from %d, waiting\n", getpid());

	for (a = 0; a < THREADS; a++)
		wait(NULL);

	printf("100 forked from %d, done\n", getpid());
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
			err(1, "fork %d", a);
			break;
		default:
			printf("forker%d %d\n", a, forkers[a]);
			break;
		}
	}

	for (a = 0; a < FORKERS; a++) {
		waitpid(forkers[a], NULL, 0);
		printf("forker%d (%d) done\n", a, forkers[a]);
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
