#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>

int main(int argc, char **argv)
{
	char buf[] = { 0xff, 0 };
	struct iovec iov = {
		.iov_base = buf,
		.iov_len = sizeof(buf),
	};
	int fd;
	_Bool do_write = argc == 2;

	if (do_write)
		puts("Doing write test");
	else
		puts("Doing open/close test");

	srand(time(NULL));

	while (1) {
		const int delta = (rand() % 200 - 100) * 100;

		fd = open("/dev/vhci", O_RDWR);
		if (fd < 0)
			err(1, "open");
		if (do_write) {
			usleep(50000 + delta / 2);

			if (writev(fd, &iov, 1) < 0)
				err(1, "writev");

			usleep(50000 + delta / 2);
		} else
			usleep(100000 + delta);

		close(fd);
	}

	return 0;
}
