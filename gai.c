#include <err.h>
#include <netdb.h>
#include <stdio.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

int main()
{
	struct addrinfo *out, *iter, hint = {
		.ai_socktype = SOCK_STREAM,
	};

	if (getaddrinfo("google.com", "http", &hint, &out) < 0)
		err(1, "gai");
	for (iter = out; iter != NULL; iter = iter->ai_next) {
		char buf[NI_MAXHOST];
		if (getnameinfo(iter->ai_addr, iter->ai_addrlen, buf,
				sizeof(buf), NULL, 0, NI_NUMERICHOST) < 0)
			err(1, "getnameinfo");
		printf("fam=%2d sock=%2d proto=%2d addr=%s\n",
				iter->ai_family,
				iter->ai_socktype, iter->ai_protocol, buf);
	}
	freeaddrinfo(out);
	return 0;
}
