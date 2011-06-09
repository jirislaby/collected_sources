#include <err.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

static void *child(void *data)
{
	unsigned int a;
	int fd = (int)(long)data;

	for (a = 0; a < 10; a++)
		write(fd, "a", 1);
	usleep(10000);
	close(fd);
	
	return NULL;
}

int main(void)
{
	struct sockaddr_in saddr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = htonl(INADDR_ANY),
		.sin_port = htons(65000)
	};
	socklen_t slen = sizeof(saddr);
	pthread_t thr;
	int fd, a;

	printf("%d\n", getpid());

	if (signal(SIGCHLD, SIG_IGN) == SIG_ERR)
		err(10, "signal SIGCHLD");

	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		err(10, "signal SIGPIPE");


	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (fd < 0)
		err(fd, "socket");

	a = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &a, sizeof(a)))
		err(1, "setsockopt");

	if (bind(fd, (struct sockaddr *)&saddr, sizeof(saddr)))
		err(2, "bind");

	if (listen(fd, SOMAXCONN))
		err(3, "listen");

	while (1) {
		a = accept(fd, (struct sockaddr *)&saddr, &slen);
		if (a < 0) {
			warn("accept");
			goto end;
		}

		if (pthread_create(&thr, NULL, child, (void *)(long)a)) {
			close(a);
			warnx("pthread_create failed");
			goto end;
		}
		pthread_detach(thr);
	}

end:
	close(fd);

	return a;
}
