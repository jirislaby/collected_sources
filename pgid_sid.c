#include <err.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

static void fump(const char *who, unsigned int a)
{
	printf("%s %u pid=%d pgid=%d sid=%d\n", who, a,
			getpid(), getpgrp(), getsid(0));
}

static void sig(int s)
{
	printf("%s: %d\n", __func__, getpid());
}

int main()
{
	unsigned int a;

	signal(SIGINT, sig);

	fump("paren", 0);

	for (a = 0; a < 10; a++)
		switch (fork()) {
		case -1:
			err(1, "fork");
		case 0:
			if (a == 5)
				setpgid(0, 0);
			if (a == 7)
				setsid();
			usleep(a * 10000);
			fump("child", a);
			sleep(4);
			return 0;
		}
	sleep(2);
	printf("killing %d\n", -getpid());
	kill(-getpid(), SIGINT);
	for (a = 0; a < 10; a++)
		wait(NULL);
	puts("died");
	return 0;
}
