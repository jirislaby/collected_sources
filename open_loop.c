#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

int main(int, char **argv)
{
	while (1) {
		int fd = open(argv[1], O_RDWR);
		if (fd < 0)
			err(1, "open");
		close(fd);
	}

	return 0;
}
