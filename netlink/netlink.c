#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <linux/netlink.h>

static void server(int fd, const struct sockaddr_nl *saddr)
{
	char buf[5];

	if (bind(fd, (const struct sockaddr *)saddr, sizeof(*saddr)) < 0)
		err(1, "bind");

	while (1) {
		ssize_t rcv = recv(fd, buf, sizeof(buf), 0);

		if (rcv < 0)
			err(1, "recv");
		if (write(1, buf, rcv) != rcv)
			warn("write %zd", rcv);
	}
}

static void client(int fd, const struct sockaddr_nl *saddr, const char *msg)
{
	size_t len = strlen(msg);
	ssize_t snd = sendto(fd, msg, len, 0, (const struct sockaddr *)saddr,
			sizeof(*saddr));
	if (snd != len)
		warn("sendto %zd", snd);
}

int main(int argc, char **argv)
{
	struct sockaddr_nl saddr = {
		.nl_family = AF_NETLINK,
	};
	pid_t pid;
	int fd;

	fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_USERSOCK);
	if (fd < 0)
		err(1, "socket");

	if (argc == 3) {
		pid = atoi(argv[1]);
	} else if (argc == 1) {
		pid = getpid();
		if (pid < 0)
			err(1, "getpid");
		printf("listening with PID=%d\n", pid);
	} else
		errx(1, "bad args");

	saddr.nl_pid = pid;

	if (argc > 1)
		client(fd, &saddr, argv[2]);
	else
		server(fd, &saddr);

	close(fd);

	return 0;
}
