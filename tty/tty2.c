#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char **argv)
{
	int fd, ld1, ld2;
	while (1) {
		fd = open(argv[1], O_RDWR);
		if (fd < 0) {
			warn("open");
			continue;
		}

		ld1 = 0; ld2 = 2;
		while (1) {
			ioctl(fd, TIOCSETD, &ld1);
			ioctl(fd, TIOCSETD, &ld2);
		}
		close(fd);
	}
}
