#include <err.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../supp.h"

static void do_io(int fd)
{
	struct pollfd pfd[2] = {
		{ .fd = STDIN_FILENO, .events = POLLIN },
		{ .fd = fd, .events = POLLIN }
	};
	char buf[65535];
	unsigned int a;

	if (write(fd, "bubak\n", 6) < 0)
		err(1, "write");

	while (1) {
		if (poll(pfd, ARRAY_SIZE(pfd), -1) <= 0)
			err(1, "poll");
		for (a = 0; a < ARRAY_SIZE(pfd); a++) {
			if (pfd[a].revents == POLLIN) {
				ssize_t rd = read(pfd[a].fd, buf, sizeof(buf));
				if (rd <= 0)
					return;
				write(pfd[a].fd == STDIN_FILENO ? fd : STDOUT_FILENO, buf, rd);
			}
			pfd[a].revents = 0;
		}
	}
}

static const char * const ldiscs[] = {
	"TTY",
	"SLIP",
	"MOUSE",
	"PPP",
	"STRIP",
	"AX25",
	"X25",
	"6PACK",
	"MASC",
	"R3964",
	"PROFIBUS_FDL",
	"IRDA",
	"SMSBLOCK",
	"HDLC",
	"SYNC_PPP",
	"HCI",
	"GIGASET_M101",
	"SLCAN",
	"PPS",
	"V253",
	"CAIF",
	"GSM0710",
	"TI_WL",
	"TRACESINK",
	"TRACEROUTER",
	"NCI",
	"SPEAKUP",
	"NULL",
};

int main(int argc, char *argv[])
{
	int fd, ld;

	printf("sz=%zu as=%zu\n", sizeof(ldiscs), ARRAY_SIZE(ldiscs));

	if (argc < 3)
		return 1;

	for (ld = 0; ld < ARRAY_SIZE(ldiscs); ld++)
		if (!strcmp(ldiscs[ld], argv[2]))
			break;

	if (ld == ARRAY_SIZE(ldiscs))
		return 2;

	printf("setting %s (%d) on %s\n", argv[2], ld, argv[1]);

	fd = open(argv[1], O_RDWR);
	if (fd < 0)
		err(1, "open");
	if (ioctl(fd, TIOCSETD, &ld))
		err(1, "ioctl");

	do_io(fd);

	close(fd);

	return 0;
}
