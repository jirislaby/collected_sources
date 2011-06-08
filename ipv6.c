#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	int ret = 0;

	argc--, argv++;

	if (!argc) {
		puts("Number of args is wrong. At least one is needed (network "
			"name or address)");
		ret = 1;
		goto out;
	}

	struct protoent *pe = getprotobyname("tcp");
	struct addrinfo *out, ai = {0, PF_UNSPEC, SOCK_STREAM, pe->p_proto, };

	if (getaddrinfo(argv[0], argv[1] ? : "10001", &ai, &out)) {
		perror("getaddrinfo error");
		ret = 100;
		goto out;
	}

	switch (out->ai_addr->sa_family) {
		case AF_INET:
			1;
			struct sockaddr_in *in =
				(struct sockaddr_in*)out->ai_addr;
			printf("ipv4: %s, %d\n", inet_ntoa(in->sin_addr),
				ntohs(in->sin_port));
			break;
		case AF_INET6:
			1;
			struct sockaddr_in6 *in6 =
				(struct sockaddr_in6*)out->ai_addr;
			char buf[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET6, (void*)&in6->sin6_addr, buf,
					sizeof(buf));
			printf("ipv6: %s, %d\n", buf, ntohs(in6->sin6_port));
			break;
		default:
			puts("unknown address class");
			ret = 2;
			goto frout;
	}

	int fd = socket(out->ai_addr->sa_family, SOCK_STREAM, pe->p_proto);
	if (fd < 0) {
		perror("socket error");
		ret = 3;
		goto frout;
	}

	if (connect(fd, out->ai_addr, out->ai_addrlen)) {
		perror("connect error");
		goto clout;
	}


	int len;
	char buf[128];
	struct timeval tv = {5, 0};
	fd_set fs;

	FD_ZERO(&fs);
	FD_SET(0, &fs);
	FD_SET(fd, &fs);

	while (select(fd + 1, &fs, NULL, NULL, &tv) >= 0) {
		if (FD_ISSET(0, &fs)) {
			if ((len = read(0, buf, sizeof(buf))))
				send(fd, buf, len, 0);
			else
				goto clout;
		}

		if (FD_ISSET(fd, &fs)) {
			if ((len = recv(fd, buf, sizeof(buf), 0)))
				write(0, buf, len);
			else {
				puts("Server ends the relation");
				goto clout;
			}
		}

		FD_ZERO(&fs);
		FD_SET(0, &fs);
		FD_SET(fd, &fs);
		tv.tv_sec = 5;
		tv.tv_usec = 0;
	}

	perror("select error");

clout:
	close(fd);

frout:	
	freeaddrinfo(out);

out:
	return ret;
}
