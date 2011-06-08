#include <stdio.h>
#define __USE_LARGEFILE64
#define _LARGEFILE64_SOURCE
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
	int fd = open("test", O_CREAT|O_WRONLY|O_LARGEFILE, 0644);
	if (fd < 0)
		return 1;

	lseek64(fd, 1ull << 40, SEEK_SET);
	write(fd, "x", 1);
	if (a) {
		puts("");
	}

	close(fd);

	return 0;
}
