#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

int main()
{
	int fd;
	char buf[4096];

	fd = open("/dev/ttyprintk0", O_RDWR);
	if (fd < 0)
		err(1, "open");

	memset(buf, 'A', sizeof(buf));
	buf[sizeof(buf) - 1] = '\r';

	if (write(fd, buf, sizeof(buf)) < 0)
		err(1, "write");

	close(fd);

	return 0;
}
