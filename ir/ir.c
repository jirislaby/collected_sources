/*
 *  Copyright (C) 2007 Jiri Slaby <jirislaby@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#define DEBUG	0

#define print2(x...)		fprintf(stderr, x)

#if DEBUG >= 2
# define pr_debug2(x...)	print2(x);
#else
# define pr_debug2(x...)	do {} while(0)
#endif

#if DEBUG >= 1
# define pr_debug(x...)		print2(x);
#else
# define pr_debug(x...)		do {} while(0)
#endif

#define setup_timer(t, cycles) do { \
	(t)->it_value.tv_usec = (cycles) > 10 ? (cycles) * 2500 : 200000; \
	setitimer(ITIMER_REAL, (t), NULL); \
} while (0)

static unsigned int timeout;

static inline unsigned long timeval_diff(struct timeval *x, struct timeval *y)
{
	unsigned int nsec;

	/* Perform the carry for the later subtraction by updating y. */
	if (x->tv_usec < y->tv_usec) {
		nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
		y->tv_usec -= 1000000 * nsec;
		y->tv_sec += nsec;
	}
	if (x->tv_usec - y->tv_usec > 1000000) {
		nsec = (x->tv_usec - y->tv_usec) / 1000000;
		y->tv_usec += 1000000 * nsec;
		y->tv_sec -= nsec;
	}

	return (x->tv_sec - y->tv_sec) * 1000000 + x->tv_usec - y->tv_usec;
}

static void timer(int s)
{
	timeout++;
	puts("NOW");
}

static inline void output_arr(unsigned long long arr)
{
	if (arr)
		printf("%.16llx\n", arr);
}

static int process(int fd)
{
	static unsigned int max_cnt;
	struct timeval start, end;
	struct itimerval itim;
	unsigned long long arr = 0;
	unsigned long diff;
	unsigned int cnt = 0;
	int ret, oret = TIOCM_DSR;

	if (ioctl(fd, TIOCMIWAIT, TIOCM_DSR)) {
		perror("ioctl (TIOCMIWAIT)");
		return 1;
	}
	if (ioctl(fd, TIOCMGET, &ret)) {
		perror("ioctl (TIOCMGET)");
		return 1;
	}
	ret &= TIOCM_DSR;

	if (ret) /* last change -- do nothing */
		return 0;

	timeout = 0;
	memset(&itim, 0, sizeof(itim));
	setup_timer(&itim, max_cnt);
	while (!timeout) {
		if (ret ^ oret) {
			if (gettimeofday(!ret ? &end : &start, NULL)) {
				perror("gettimeofday");
				return 1;
			}
			if (!ret) {
				diff = timeval_diff(&end, &start);
				if (diff > 3000) {
					if (cnt > 0) {
						output_arr(arr);
						max_cnt = MAX(max_cnt, cnt);
						setup_timer(&itim, max_cnt);
					}
					cnt = 0;
					arr = 0;
				} else if (diff > 1000 && diff < 2500 && cnt<64)
					arr |= 1ULL << cnt;
				cnt++;
				pr_debug2("t_po=%4lu (%u)\n", diff, cnt);
			}
		}
		oret = ret;

		if (ioctl(fd, TIOCMGET, &ret)) {
			perror("ioctl (TIOCMGET)");
			return 1;
		}
		ret &= TIOCM_DSR;
	}
	if (gettimeofday(&start, NULL)) {
		perror("gettimeofday");
		return 1;
	}
	if ((diff = timeval_diff(&start, &end)) < 1000) {
		printf("new controller: %lu (%u/%u)\n", diff, max_cnt, max_cnt * 2500);
		max_cnt = 0;
	} else
		max_cnt = MAX(max_cnt, cnt);

	output_arr(arr);
	printf("-- %u/%u\n", cnt, max_cnt);

	return 0;
}

int main(void)
{
	int fd;

	fd = open("/dev/ttyS1", O_RDWR|O_NONBLOCK);
	if (fd < 0) {
		perror("open");
		goto err;
	}
	pr_debug("opened\n");

	if (signal(SIGALRM, timer)) {
		perror("signal handler");
		goto err_cl;
	}

	while (!process(fd))
		;

	close(fd);

	return 0;
err_cl:
	close(fd);
err:
	return 1;
}
