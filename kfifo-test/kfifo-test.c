#include <assert.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/wait.h>

#define FIFO
//#define DUMP
#define TIMEOUT 3
#define ERRORS 2

#define roundup(x, y) ({				\
	typeof(y) __y = y;                              \
	(((x) + (__y - 1)) / __y) * __y;                \
})

static void dump(int fd, const char *buf, unsigned int size)
{
	static unsigned long chunk;
	static unsigned long off;
	static unsigned int errors;
	static char last = 0;

	unsigned int a;

	for (a = 0; a < size; a++) {
		char c = buf[a];
#ifdef DUMP
		putchar(c);
#endif
		if (c - '0' != last) {
			warnx("received '%c' (0x%.2x), expected '%c' at offset %lu (chunk %lu)",
					isprint(c) ? c : ' ', c, last + '0', off, chunk);
			ioctl(fd, 0xdea1);
			last = c - '0';
			if (errors++ > ERRORS)
				errx(1, "too many errors");
		}

		last++;
		last %= 10;
		off++;
	}

#ifdef DUMP
	puts("");
	fflush(stdout);
#endif
	fflush(stderr);
	chunk++;
}

static bool __do_poll(int fd, short mask, const char *mask_str)
{
	struct pollfd fds = {
		.fd = fd,
		.events = mask,
	};
	int ret;

	ret = poll(&fds, 1, TIMEOUT * 1000);
	if (ret < 0)
		err(1, "poll(%s)", mask_str);
	if (!ret) {
		warnx("poll(%s): no data", mask_str);
		return false;
	}

	if (!(fds.revents & mask))
		errx(1, "no %s", mask_str);

	return true;
}

#define do_poll(fd, mask) __do_poll(fd, mask, #mask)

static void do_read(int fd, unsigned int fifo_size)
{
	char buf[fifo_size / 8];
	ssize_t rd;

	while (1) {
		if (!do_poll(fd, POLLIN))
			break;

		rd = read(fd, buf, sizeof(buf));
		if (rd < 0)
			err(1, "read");

		if (!rd)
			errx(1, "nothing read");

		dump(fd, buf, rd);
	}
}

static void do_write(int fd, unsigned int fifo_size)
{
	unsigned char prefill[roundup(fifo_size, 10)];
	unsigned int a, off = 0;
	ssize_t wr;

	for (a = 0; a < sizeof(prefill); a++)
		prefill[a] = '0' + a % 10;

	assert(prefill[sizeof(prefill) - 1] == '9');

	while (1) {
		unsigned int cnt = sizeof(prefill) - off;
		if (!cnt) {
			off = 0;
			continue;
		}

		if (!do_poll(fd, POLLOUT))
			break;

		wr = write(fd, &prefill[off], cnt);
		if (wr < 0)
			err(1, "write");

		if (!wr)
			errx(1, "nothing written");

		off += wr;
	}
}

int main(int, char **argv)
{
	int fd, fifo_size;

	fd = open(argv[1], O_RDWR);
	if (fd < 0)
		err(1, "open");

	fifo_size = ioctl(fd, 0xfffa);
	if (fifo_size < 0)
		err(1, "ioctl");

	printf("%s: fifo_size=%d\n", __func__, fifo_size);

	switch (fork()) {
	case 0:
		do_read(fd, fifo_size);
		break;
	default:
		do_write(fd, fifo_size);
		wait(NULL);
		break;
	case -1:
		err(1, "fork");
	}

	close(fd);

	return 0;
}

