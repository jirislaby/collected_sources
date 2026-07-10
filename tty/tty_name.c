#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	int fd = STDIN_FILENO;//open("/dev/tty0", O_RDONLY);
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

