#include <err.h>
#include <fcntl.h>
#include <getopt.h>
#include <poll.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "tty.h"

static volatile sig_atomic_t stop;

static void sig(int)
{
	stop = true;
}

int main(int argc, char **argv)
{
	static const struct option opts[] = {
		{ "counter", 0, NULL, 'c' },
		{ "noecho", 0, NULL, 'n' },
		{ "nosignal", 0, NULL, 1 },
		{ "openonce", 0, NULL, 'o' },
		{ "read", 0, NULL, 'r' },
		{ "silent", 0, NULL, 's' },
		{ "sleep", 1, NULL, 'l' },
		{ "write", 2, NULL, 'w' },
		{ }
	};
	ssize_t rd;
	int fd = -1, opt;
	char buf[64];
	bool do_counter = false, noecho = false, nosignal = false;
	bool open_once = false, do_read = false, silent = false;
	char *do_write = NULL;
	size_t write_len = 0;
	unsigned int counter = 0, sleep = 0;

	while ((opt = getopt_long(argc, argv, "cnl:orsw::", opts, NULL)) >= 0) {
		switch (opt) {
		case '?':
			errx(1, "bad option");
		case 1:
			nosignal = true;
			break;
		case 'c':
			do_counter = true;
			break;
		case 'e':
			noecho = true;
			break;
		case 'l':
			sleep = atoi(optarg);
			break;
		case 'o':
			open_once = true;
			break;
		case 'r':
			do_read = true;
			break;
		case 's':
			silent = true;
			break;
		case 'w':
			do_write = optarg ? : "x";
			write_len = strlen(do_write);
			break;
		}
	}

	if (!do_read && !do_write)
		errx(1, "no read and write");

	if (do_counter && !do_write)
		errx(1, "counter with no write");

	if (optind == argc)
		errx(1, "no device to read");

	if (do_counter) {
		do_write = strdup(do_write);
		do_write = realloc(do_write, write_len + 8 + 1 + 1);
	}

	if (!nosignal) {
		if (signal(SIGTERM, sig))
			err(1, "signal(SIGTERM)");
		if (signal(SIGINT, sig))
			err(1, "signal(SIGINT)");
	}

	if (!silent) {
		printf("Going to open %s ", open_once ? "once" : "repeatedly");
		if (do_read)
			printf("READ ");
		if (do_write)
			printf("WRITE ");
		if (do_counter)
			printf("with counter ");
		if (sleep)
			printf("with sleep %u ms ", sleep);
		puts("");
	}

	while (1) {
		struct pollfd pfd = {
			.events = POLLERR,
		};
		if (fd < 0 || !open_once) {
			fd = open(argv[optind], O_RDWR);
			if (fd < 0) {
				warn_on_EPERM("open");
				if (!silent)
					write(1, "O", 1);
				continue;
			}
		}
		if (do_write)
			pfd.events |= POLLOUT;
		if (do_read)
			pfd.events |= POLLIN;
		pfd.fd = fd;
		if (poll(&pfd, 1, -1) < 0 || pfd.revents & POLLERR)
			putchar('P');
		if (pfd.revents & POLLOUT) {
			size_t write_len2 = write_len;
			size_t written;

			if (do_counter)
				write_len2 += sprintf(do_write + write_len, "%u\n", ++counter);
			written = write(fd, do_write, write_len2);
			if (written != write_len2) {
				if (errno == EIO) {
					close(fd);
					fd = -1;
					if (!silent)
						write(1, "w", 1);
					continue;
				}
				if (!silent) {
					printf("\nW(%zu != %zu)\n", written, write_len2);
					write(1, "W", 1);
				}
			}
		}
		if (pfd.revents & POLLIN) {
			rd = read(fd, buf, sizeof(buf));
			if (rd < 0) {
				if (errno == EIO) {
					close(fd);
					fd = -1;
					if (!silent)
						write(1, "r", 1);
					continue;
				}
				if (!silent)
					write(1, "R", 1);
			}
			if (!noecho && rd > 0)
				write(1, buf, rd);
		}

		if (!open_once)
			close(fd);

		if (!silent)
			write(1, "y", 1);
		if (stop)
			break;
		if (sleep)
			usleep(sleep * 1000);
	}


	if (!silent)
		printf("\nClosing (counter=%u)\n", counter);

	if (open_once)
		close(fd);

	if (do_counter)
		free(do_write);

	return 0;
}
