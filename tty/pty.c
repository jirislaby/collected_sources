#define _XOPEN_SOURCE
#define _GNU_SOURCE
#include <err.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char **argv)
{
	char path[PATH_MAX];
	int fd, ptmx, rnd;
	while (1) {
		ptmx = open("/dev/ptmx", O_RDWR);
		if (ptmx < 0)
			err(1, "open ptmx");

		if (grantpt(ptmx))
			err(1, "grantpt");

		if (unlockpt(ptmx))
			err(1, "unlockpt");

		if (ptsname_r(ptmx, path, sizeof(path)))
			err(1, "ptsname");

		fd = open(path, O_RDWR);
		if (fd < 0)
			err(1, "open slave");

		if (write(fd, "ahoj", 4) != 4)
			err(1, "write");
		if (read(ptmx, path, 4) != 4)
			err(1, "read");
		if (strncmp("ahoj", path, 4))
			errx(1, "strcmp");

		rnd = rand();
		usleep(150 * (1 + rnd % 80));
		if (rnd & 0x40000000) {
			close(fd);
			close(ptmx);
		} else {
			close(ptmx);
			close(fd);
		}
	}
	return 0;
}
