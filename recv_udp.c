#include <err.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

int main()
{
/*	struct sockaddr_in sin = {
		.sin_family = AF_INET,
		.sin_port = htons(10000),
		.sin_addr = { INADDR_ANY },
	};*/
	struct sockaddr_storage sa;
	socklen_t slen = sizeof(sa);
	char buf[128];
	int fd = socket(AF_INET, SOCK_DGRAM, 0);

//	bind(fd, (struct sockaddr *)&sin, sizeof(sin));

//	sendto(fd, buf, 1, 0, (struct sockaddr *)&sin, sizeof(sin));
//	write(fd, "", 0);
	getsockname(fd, (struct sockaddr *)&sa, &slen);
	recv(fd, buf, sizeof(buf), 0);

	return 0;
}
