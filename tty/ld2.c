#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
	int fd, ld1, ld2;

	fd = open(argv[1], O_RDWR);
	if (fd < 0)
		err(1, "open");

	ld1 = atoi(*++argv);
	ld2 = atoi(*++argv);

	while (1) {
		if (ioctl(fd, TIOCSETD, &ld1))
			err(1, "ioctl");
		if (ioctl(fd, TIOCSETD, &ld2))
			err(1, "ioctl");
	}
	close(fd);
}
