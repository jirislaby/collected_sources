#define _GNU_SOURCE
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

static unsigned int bytes = 0;

void update(int sig)
{
	char *buf = NULL, *ptr;
	size_t read;

	FILE *file = fopen("/proc/net/dev", "r");
	if (!file) {
		perror("proc/net/dev open");
		goto ret;
	}

	while (getline(&buf, &read, file) >= 0) {
		if ((ptr = strstr(buf, "eth0"))) {
			bytes = strtoul(ptr + 5, NULL, 10) >> 20;
			free(buf);
			break;
		}

		free(buf);
		buf = NULL;
	}

	fclose(file);
ret:
	alarm(3600);
}

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

	if (fork())
		return 0;

	signal(SIGALRM, update);
	alarm(1);

	struct protoent *pe = getprotobyname("tcp");
	struct addrinfo *out, ai = {0, PF_UNSPEC, SOCK_STREAM, pe->p_proto, };

	if (getaddrinfo(argv[0], argv[1] ? : "80", &ai, &out)) {
		perror("getaddrinfo error");
		ret = 100;
		goto out;
	}

	int fd = socket(out->ai_addr->sa_family, SOCK_STREAM, pe->p_proto);
	if (fd < 0) {
		perror("socket error");
		ret = 3;
		goto frout;
	}

	if (bind(fd, out->ai_addr, out->ai_addrlen)) {
		perror("bind error");
		goto clout;
	}

	if (listen(fd, 5)) {
		perror("listen error");
		goto clout;
	}

	int len, conn;
	char buf[64];
	struct sockaddr addr;
	socklen_t addrlen = sizeof(addr);

	while ((conn = accept(fd, &addr, &addrlen)) != -1) {
		len = sprintf(buf, "HTTP/1.1 200 OK\r\nExpires: 0\r\n\r\n"
			"%.3fG\r\n", bytes / 1024.0);
		write(conn, buf, len);
		close(conn);
	}

	perror("accept error");

clout:
	close(fd);

frout:	
	freeaddrinfo(out);

out:
	return ret;
}
