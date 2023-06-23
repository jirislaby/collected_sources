#include <err.h>

#include <netdb.h>
#include <stdio.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

void loop(int fd)
{
	int len;
	char buf[128*1024];
	fd_set fs;

	FD_ZERO(&fs);
	FD_SET(STDIN_FILENO, &fs);
	FD_SET(fd, &fs);

	while (select(fd + 1, &fs, NULL, NULL, NULL) >= 0) {
		if (FD_ISSET(STDIN_FILENO, &fs)) {
			if ((len = read(STDIN_FILENO, buf, sizeof(buf))))
				send(fd, buf, len, 0);
			else
				return;
		}

		if (FD_ISSET(fd, &fs)) {
			if ((len = recv(fd, buf, sizeof(buf), 0)))
				write(STDOUT_FILENO, buf, len);
			else {
				fprintf(stderr, "Server ended the relation\n");
				return;
			}
		}

		FD_ZERO(&fs);
		FD_SET(STDIN_FILENO, &fs);
		FD_SET(fd, &fs);
	}

	perror("select error");
}

int main(int argc, char **argv)
{
	struct protoent *pe = getprotobyname("tcp");
	struct addrinfo *out, ai = {
		.ai_family = PF_UNSPEC,
		.ai_socktype = SOCK_STREAM,
		.ai_protocol = pe->p_proto,
	};
	int sock;

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

	if (connect(sock, out->ai_addr, out->ai_addrlen))
		err(1, "connect");

	freeaddrinfo(out);

	loop(sock);

	close(sock);

	return 0;
}
