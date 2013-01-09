#include <err.h>
#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

static pid_t tzap_pid = -1;

static int run_tzap(char *channel)
{
	pid_t pid;
	int fd;

	switch ((pid = vfork())) {
	case -1:
		err(3, "vfork");
	case 0: {
		char *argv[] = { "tzap", "-rc",
			"/home/xslaby/.mplayer/channels.conf", channel, NULL };
		char *env[] = { NULL };
		execve("/usr/bin/tzap", argv, env);
		err(3, "execve");
	}
	}
	tzap_pid = pid;
	fd = open("/dev/dvb/adapter0/dvr0", O_RDONLY);
	if (fd < 0)
		err(3, "open(dvr)");
	return fd;
}

static void kill_tzap(void)
{
	if (tzap_pid >= 0)
		kill(tzap_pid, SIGTERM);
}

static int handle_conn(int conn, struct sockaddr *ss, socklen_t ss_size)
{
	struct pollfd pollfd[2] = {
		{ .fd = conn, .events = POLLIN },
		{ .events = POLLIN }
	};
	nfds_t pollfds = 1;
	size_t r;
	char buf[128];

	while (1) {
		if (poll(pollfd, pollfds, 1000) < 0)
			err(2, "poll");
		if (tzap_pid >= 0) {
			pid_t ch = waitpid(tzap_pid, NULL, WNOHANG);
			if (ch < 0)
				err(2, "waitpid");
			if (ch)
				tzap_pid = -1;
		}
		if (pollfd[0].revents & (POLLERR | POLLHUP | POLLNVAL))
			return 1;
		if (pollfd[1].revents & (POLLERR | POLLHUP | POLLNVAL))
			return 1;
		if (pollfd[0].revents & POLLIN) {
			r = read(conn, buf, sizeof(buf) - 1);
			if (r < 0)
				err(1, "read from remote");
			if (!r)
				break;
			if (buf[r - 1] == '\n') {
				buf[r - 1] = 0;
				pollfd[1].fd = run_tzap(buf);
				pollfds = 2;
			}
		}
		if (pollfd[1].revents & POLLIN) {
			size_t w;
			r = read(pollfd[1].fd, buf, sizeof(buf) - 1);
			if (r < 0)
				err(1, "read from dvr");
			if (!r)
				break;
			w = write(conn, buf, r);
			if (w < 0)
				err(1, "write to remote");
			if (w != r)
				break;
		}
	}

	close(conn);

	kill_tzap();

	warnx("Connection closed");

	return 0;
}

int main(int argc, char **argv)
{
	struct protoent *pe = getprotobyname("tcp");
	struct addrinfo *out, ai = {0, PF_UNSPEC, SOCK_STREAM, pe->p_proto, };
	struct sockaddr_storage ss;
	socklen_t ss_size = sizeof(ss);
	int sock, conn, a = 1;

	argc--, argv++;

	if (!argc)
		errx(1, "Number of args is wrong. At least one is "
			"needed (network name or address)");

	if (getaddrinfo(argv[0], argv[1] ? : "10001", &ai, &out))
		err(1, "getaddrinfo error");

	switch (out->ai_family) {
	case AF_INET: {
		struct sockaddr_in *in =
			(struct sockaddr_in*)out->ai_addr;
		fprintf(stderr, "ipv4: %s, %d\n", inet_ntoa(in->sin_addr),
			ntohs(in->sin_port));
		break;
	}
	case AF_INET6: {
		struct sockaddr_in6 *in6 =
			(struct sockaddr_in6*)out->ai_addr;
		char buf[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET6, (void*)&in6->sin6_addr, buf,
				sizeof(buf));
		fprintf(stderr, "ipv6: %s, %d\n", buf, ntohs(in6->sin6_port));
		break;
	}
	default:
		errx(1, "unknown address class");
	}

	sock = socket(out->ai_family, out->ai_socktype, out->ai_protocol);
	if (sock < 0)
		err(1, "socket");

	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &a, sizeof(a)) == -1)
		err(1, "setsockopt");

	if (bind(sock, out->ai_addr, out->ai_addrlen) == -1)
		err(1, "bind");

	if (listen(sock, 10))
		err(1, "listen");

	while ((conn = accept(sock, (struct sockaddr *)&ss, &ss_size)) != -1)
		if (handle_conn(conn, (struct sockaddr *)&ss, ss_size))
			break;

	if (conn < 0)
		err(1, "accept");
			
	close(sock);

	return 0;
}
