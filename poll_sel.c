#include <err.h>
#include <poll.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/select.h>

int main(int argc, char **)
{
	int fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (argc > 2) {
		struct epoll_event epe[] = {
			{ .events = EPOLLIN, .data.fd = 0 },
			{ .events = EPOLLIN, .data.fd = 2 },
			{ .events = EPOLLIN /*| EPOLLOUT*/, .data.fd = fd },
		};
		int ep = epoll_create1(0);
		int a, b;
		for (a = 0; a < 3; a++)
			if (epoll_ctl(ep, EPOLL_CTL_ADD, epe[a].data.fd,
						&epe[a]) < 0)
				err(1, "epoll_ctl");
		a = epoll_wait(ep, epe, 3, -1);
		for (b = 0; b < a; b++)
			printf("some event: %d ready for %x!\n", epe[b].data.fd, epe[b].events);
	} else if (argc > 1) {
		struct pollfd pfd[] = {
			{ .fd = 0, .events = POLLIN },
			{ .fd = 2, .events = POLLIN },
			{ .fd = fd, .events = POLLIN | POLLOUT }
		};
		if (poll(pfd, 3, -1) > 0)
			printf("some event: %d ready for %d!\n", pfd[2].fd, pfd[2].revents);
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
