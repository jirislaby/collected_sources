#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int /*argc*/, char *argv[])
{
	int fd, ld;

	ld = atoi(argv[2]);

	fd = open(argv[1], O_RDWR);
	if (fd < 0)
		err(1, "open");
	if (ioctl(fd, TIOCSETD, &ld))
		err(1, "ioctl");
	close(fd);

	return 0;
}
