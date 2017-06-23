#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char **argv)
{
	size_t rd;
	int fd;
	char buf[64];

	while (1) {
		fd = open(argv[1], O_RDWR);
		if (fd < 0) {
//			warn("open");
			continue;
		}
		write(fd, "x", 1);
		rd = read(fd, buf, sizeof(buf));
		if (rd > 0)
			write(1, buf, rd);

		close(fd);
		write(1, "y", 1);
	}
	return 0;
}
