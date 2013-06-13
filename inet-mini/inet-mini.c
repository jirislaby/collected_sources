#include <err.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

static void handle_con(int fd)
{
	struct sockaddr_in from;
	socklen_t fromlen = sizeof(from);

	if (dup2(fd, 0) < 0)
		err(2, "dup2(0)");

	if (getpeername(0, (struct sockaddr *)&from, &fromlen) < 0)
		err(2, "getpeername");

	if (dup2(fd, 1) < 0)
		err(2, "dup2(1)");

	close(2);

	if (dup2(fd, 2) < 0)
		err(2, "dup2(2)");

	execl("/usr/sbin/in.rlogind", "in.rlogind", NULL);

	err(2, "exec");
}

int main(void)
{
	struct sockaddr_in saddr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = htonl(INADDR_ANY),
		.sin_port = htons(513)
	};
	socklen_t slen = sizeof(saddr);
	int fd, a;

	close(0);
	close(1);

	if (signal(SIGCHLD, SIG_IGN) == SIG_ERR)
		err(1, "signal SIGCHLD");

	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		err(1, "signal SIGPIPE");

	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (fd < 0)
		err(1, "socket");

	a = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &a, sizeof(a)))
		err(1, "setsockopt");

	if (bind(fd, (struct sockaddr *)&saddr, sizeof(saddr)))
		err(1, "bind");

	if (listen(fd, 1))
		err(1, "listen");

	a = accept(fd, (struct sockaddr *)&saddr, &slen);
	if (a < 0)
		err(1, "accept");

	handle_con(a);

	close(a);
	close(fd);

	return 0;
}
