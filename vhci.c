#include <err.h>
#include <errno.h>
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
		const unsigned int sched_delay = 10 * 1000;
		const unsigned int delta_jitter = sched_delay / 200;
		const unsigned int delta_multip = delta_jitter < 100 ? 1 :
			delta_jitter / 100;
		const int delta = (rand() % (delta_jitter * 2) - delta_jitter) *
			delta_multip;
		const unsigned int divisor = do_write ? 2 : 1;
		const unsigned int delay = (sched_delay + delta) / divisor;

		fd = open("/dev/vhci", O_RDWR);
		if (fd < 0)
			err(1, "open");

		usleep(delay);

		if (do_write) {
			ssize_t ret = writev(fd, &iov, 1);
			if (ret < 0 && errno != EBADFD)
				err(1, "writev");
			if (ret >= 0 && ret != 2)
				errx(1, "writev didn't return 2: %zd", ret);

			usleep(delay);
		}

		close(fd);
	}

	return 0;
}
