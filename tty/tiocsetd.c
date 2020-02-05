#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../supp.h"

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
	char buf[65535];

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

	if (write(fd, "bubak\n", 6) < 0)
		err(1, "write");

	while (1) {
		ssize_t rd = read(fd, buf, sizeof(buf));
		if (rd < 0)
			break;
	}
	close(fd);

	return 0;
}
