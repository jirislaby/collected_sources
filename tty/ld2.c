#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../util.h"

int main(int /*argc*/, char *argv[])
{
	int fd, ld1, ld2;

	fd = open(argv[1], O_RDWR);
	if (fd < 0)
		err(1, "open");

	ld1 = atoi(argv[2]);
	ld2 = atoi(argv[3]);

	printf("Switching between %d, %d\n", ld1, ld2);

	while (1) {
		if (ioctl(fd, TIOCSETD, &ld1))
			warn("ioctl");
		rand_sched();
		if (ioctl(fd, TIOCSETD, &ld2))
			warn("ioctl");
		rand_sched();
	}
	close(fd);
}
