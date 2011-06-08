#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(void)
{
	int fd, a;

	fd = open("/dev/ttyS0", O_RDWR);
	if (fd < 0)
		err(1, "open");

	if (ioctl(fd, TCFLSH, TCIOFLUSH))
		err(2, "TCFLSH");

/*	if (ioctl(fd, TIOCMGET, &a))
		err(3, "TIOCMGET");

	if (ioctl(fd, TIOCMSET, &a))
		err(3, "TIOCMSET");*/

	write(fd, "\360", 1);

	read(fd, &a, 1);

	close(fd);

	return 0;
}
