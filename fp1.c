#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

static void do_fp(pid_t pid, int sig)
{
	unsigned int i;
	long int a, b;
	double c, d;

	a = rand() % 1000000;
	b = rand() % 1000000;

	for (i = 0; i < 10000000; i++) {
		a++;
		b++;
		c = a;
		d = b;
		if (!(i % 10000000)) {
			putchar('0' + pid % 10);
			fflush(stdout);
		}
		if (a + b != (long)(c + d))
			printf("\n%u %u %u %6ld + %6ld = %7ld != %7ld = %7.0F = %6.0F + %6.0F\n",
					i, pid, sig,
					a, b, a + b, (long)(c + d), c + d, c, d);
		if (a * b != (long)(c * d))
			printf("\n%u %u %u %6ld * %6ld = %7ld != %7ld = %7.0F = %6.0F * %6.0F\n",
					i, pid, sig,
					a, b, a * b, (long)(c * d), c * d, c, d);
		double rad = (double)(a % 180) / 180 * M_PI / 2;
		if (fabs(asin(sin(rad)) - rad) > 0.00001)
			printf("\n%u %u %u asin(sin(%3ld = %5.3F)) = %7.2F != %7.2F (sin = %5.2F)\n",
					i, pid, sig,
					a % 180,
					rad, asin(sin(rad)), rad, sin(rad));
	}
}

static void child(void)
{
	pid_t pid = getpid();

	srand(pid);

	while (1)
		do_fp(pid, 0);
}

static void sig(int)
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
