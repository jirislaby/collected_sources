#include <signal.h>
#include <stdio.h>

int main()
{
	sigset_t s;
	int sig, ret;

	sigfillset(&s);
	sigaddset(&s, SIGTERM);
	sigprocmask(SIG_BLOCK, &s, NULL);
	do {
		ret = sigwait(&s, &sig);
		printf("got: %d ret=%d\n", sig, ret);
	} while (sig != SIGHUP);
	return 0;
}
