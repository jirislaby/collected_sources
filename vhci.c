#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>

int main()
{
	char buf[] = { 0xff, 0 };
	struct iovec iov = {
		.iov_base = buf,
		.iov_len = sizeof(buf),
	};
	int fd;

	fd = open("/dev/vhci", O_RDWR);
	if (fd < 0)
		err(1, "open");

	if (writev(fd, &iov, 1) < 0)
		err(1, "writev");

	close(fd);
	return 0;
}
