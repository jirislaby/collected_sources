#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netdb.h>
#include <netinet/in.h>

#include <sys/types.h>
#include <sys/socket.h>

#define printf2(args...)	fprintf(stderr, args)

int main(void)
{
	const static struct addrinfo ainfo = {
		.ai_family = PF_UNSPEC,
		.ai_socktype = SOCK_STREAM,
		.ai_protocol = IPPROTO_TCP,
	};
	struct addrinfo *ai = NULL;
	FILE *fsock;
	size_t bufs = 0;
	int retval, sock;
	char *buf = NULL;

	retval = getaddrinfo("localhost", "6600", &ainfo, &ai);
	if (retval) {
		retval = 1;
		printf2("getaddrinfo: %s\n", gai_strerror(retval));
		goto err;
	}
	retval = 1;

	sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	if (!sock) {
		perror("socket");
		goto err_fr;
	}

	if (connect(sock, ai->ai_addr, ai->ai_addrlen)) {
		perror("connect");
		goto err_cl;
	}

	freeaddrinfo(ai);
	ai = NULL;

	fsock = fdopen(sock, "rw");
	if (fsock == NULL) {
		perror("fdopen");
		goto err_cl;
	}

	sock = -1;

	while (getline(&buf, &bufs, fsock) >= 0) {
		printf("%s", buf);
		if (strncmp(buf, "OK", 2) == 0)
			break;
	}

	free(buf);

	retval = 0;

	fclose(fsock);
err_cl:
	if (sock >= 0)
		close(sock);
err_fr:
	if (ai != NULL)
		freeaddrinfo(ai);
err:
	return retval;
}
