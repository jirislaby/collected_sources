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

	while (1) {
		fd = open("/dev/vhci", O_RDWR);
		if (fd < 0)
			err(1, "open");

		usleep(50);

		if (writev(fd, &iov, 1) < 0)
			err(1, "writev");

		usleep(50);

		close(fd);
	}

	return 0;
}
