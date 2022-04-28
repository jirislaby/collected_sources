#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

int main(int /*argc*/, char **argv)
{
	int fd;
	while (1) {
		fd = open(argv[1], O_RDWR);
		if (fd < 0) {
//			warn("open");
			continue;
		}
//		usleep(150 * (1 + rand() % 80));
		close(fd);
	}
	return 0;
}
