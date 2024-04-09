#include <err.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

static void *child(void *)
{
	struct sockaddr_in saddr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = htonl(0x7f000001),
		.sin_port = htons(65000)
	};
	unsigned int a = 0;
	int fd, r;
	char ch;

	while (1) {
		fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
		if (fd < 0)
			err(fd, "socket");

		if (connect(fd, (struct sockaddr *)&saddr, sizeof(saddr)))
			err(1, "connect");

		while ((r = read(fd, &ch, 1)) == 1)
			if (!(a++ % 100))
				putchar(ch);

		if (r < 0)
			warn("read: %d", r);
		close(fd);
		if (!(a % 1000)) {
			printf("%u\n", a);
			fflush(stdout);
		}
		pthread_testcancel();
	}

	return NULL;
}

#define THREADS 200

int main(void)
{
	pthread_t thr;
	unsigned int a;

	for (a = 0; a < THREADS; a++)
		if (pthread_create(&thr, NULL, child, NULL))
			errx(1, "pthread create failed %u", a);
		else if (a != THREADS - 1)
			pthread_detach(thr);

	pthread_join(thr, NULL);
	pthread_detach(thr);

	return 0;
}
