#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	int fd = open("/dev/tty0", O_RDONLY);
	char *name;

	if (fd < 0)
		err(1, "open");
	name = ttyname(fd);
	if (!name)
		err(1, "ttyname");

	printf("%s\n", name);

	free(name);

	close(fd);

	return 0;
}

