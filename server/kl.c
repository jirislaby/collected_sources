#include <err.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>

int main(int argc, char **argv)
{
	pid_t p;

	if (argc < 2)
		return 1;

	p = atoi(argv[1]);

	while (1) {
		if (kill(p, SIGSTOP))
			err(2, "kill");
		usleep(1); /* sleep for ~ 1/HZ */
		if (kill(p, SIGCONT))
			err(2, "kill");
		usleep(1); /* sleep for ~ 1/HZ */
	}

	return 0;
}
