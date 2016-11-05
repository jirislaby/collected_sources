#include <stdlib.h>
#include <unistd.h>

#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#include <linux/tty.h>

static void km()
{
	int r0 = open("/dev/ptmx", O_RDONLY);
	int val = N_PPS;
	ioctl(r0, TIOCSETD, &val); // TIOCSETD
	exit(0);
}

#define P 15

int main()
{
	int a;

	while (1) {
		for (a = 0; a < P; a++)
			if (!fork())
				km();

		for (a = 0; a < P; a++)
			wait(NULL);

		usleep(200000);
	}
}
