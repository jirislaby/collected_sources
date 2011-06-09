#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
	int fd, ld;

	fd = open(argv[1], O_RDWR);
	if (fd < 0)
		err(1, "open");

	argc -= 2;
	argv += 2;

	while (argc--) {
		ld = atoi(*argv++);

		if (ioctl(fd, TIOCSETD, &ld))
			err(1, "ioctl");

		if (argc)
			sleep(2);
	}
	close(fd);
}
