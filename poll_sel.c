#include <poll.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/select.h>

int main(int argc, char **argv)
{
	int fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (argc > 1) {
		struct pollfd pfd[] = { { 0, POLLIN }, {2, POLLIN}, { fd, POLLIN | POLLOUT } };
		if (poll(pfd, 3, -1) > 0)
			printf("some event: %d ready for %d!", pfd[2].fd, pfd[2].revents);
	} else {
		fd_set rd, wr;
		FD_ZERO(&rd); FD_ZERO(&wr);
		FD_SET(0, &rd); FD_SET(2, &rd); FD_SET(fd, &rd);
		FD_SET(fd, &wr);
		if (select(fd + 1, &rd, &wr, NULL, NULL) > 0)
			printf("some event %d!", FD_ISSET(fd, &wr));
	}
	return 0;
}
