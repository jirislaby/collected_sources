#include <err.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>

int main()
{
	char buf[128];
	int sock[2];
	ssize_t rd;

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sock) < 0)
		err(1, "socketpair");

/*	switch (fork()) {
	case 0:*/
		write(sock[1], "ahoj", 4);
		write(sock[1], "vogo", 4);
		close(sock[1]);
		while ((rd = read(sock[0], buf, sizeof(buf))) > 0)
			printf("r=%.*s\n", (int)rd, buf);
		close(sock[0]);
/*		break;
	default:
		close(sock[1]);
		usleep(100000);
		while ((rd = read(sock[0], buf, sizeof(buf))) > 0)
			printf("r=%.*s\n", (int)rd, buf);
		close(sock[0]);
		wait(NULL);
		break;
	case -1:
		err(1, "fork");
	}*/

	return 0;
}
