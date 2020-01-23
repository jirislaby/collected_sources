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
	int fd, ld;
	char buf[65535];

	ld = N_HDLC;

	fd = open(argv[1], O_RDWR);
	if (fd < 0)
		err(1, "open");
	if (ioctl(fd, TIOCSETD, &ld))
		err(1, "ioctl");

	if (write(fd, "bubak\n", 6) < 0)
		err(1, "write");

	while (1) {
		ssize_t rd = read(fd, buf, sizeof(buf));
		if (rd < 0)
			break;
	}
	close(fd);

	return 0;
}
