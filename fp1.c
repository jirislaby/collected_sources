#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

static void do_fp(pid_t pid, int sig)
{
	static unsigned int cnt;
	unsigned int i;
	int a, b;
	double c, d;

	for (i = 0; i < 10000000; i++) {
		a = rand() % 1000000;
		b = rand() % 1000000;
		c = a;
		d = b;
		if (!(++cnt % 10000000) || a + b != (int)(c + d))
			printf("%u %u %u %6d + %6d = %7d != %7d = %7.0F = %6.0F + %6.0F\n",
					!(cnt % 10000000), pid, sig,
					a, b, a + b, (int)(c + d), c + d, c, d);
	}
}

static void child(void)
{
	pid_t pid = getpid();

	srand(pid);

	while (1)
		do_fp(pid, 0);
}

static void sig(int signr)
{
//	do_fp(getpid(), 1);
}

int main(void)
{
	unsigned int a;

	signal(SIGUSR1, sig);

	for (a = 0; a < 10; a++)
		if (!fork())
			child();
	wait(NULL);

	return 0;
}
