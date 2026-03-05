#define _GNU_SOURCE
#include <err.h>
#include <stdio.h>
#include <unistd.h>

#include <sched.h>
#include <sys/wait.h>

#if 1
#define FORKERS 20
#define YIELDS 100000000
#else
#define FORKERS 1
#define YIELDS 1
#endif

static void yi()
{
#if 0
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(1, &mask);
	CPU_SET(2, &mask);

	if (sched_setaffinity(0, sizeof(mask), &mask))
		err(1, "sched_setaffinity");
#endif

	for (unsigned i = 0; i < YIELDS; ++i) {
		for (unsigned i = 0; i < 1000; ++i)
			asm volatile("nop" ::: "memory");
		//sched_yield();
	}
}

int main()
{
	pid_t forkers[FORKERS];
	unsigned a;

	for (a = 0; a < FORKERS; a++) {
		switch ((forkers[a] = fork())) {
		case 0:
			yi();
			return 0;
		case -1:
			err(1, "DIE fork of %d'th forker", a);
			break;
		default:
			break;
		}
	}

	yi();

	for (a = 0; a < FORKERS; a++)
		waitpid(forkers[a], NULL, 0);

	return 0;
}
