#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <linux/kd.h>
#include <linux/keyboard.h>

static void get(int fd)
{
	unsigned i;

	for (i = 0; i < MAX_NR_FUNC; i++) {
		struct kbsentry kbs = {
			.kb_func = i,
		};
		if (ioctl(fd, KDGKBSENT, &kbs) < 0)
			err(1, "ioctl(KDGKBSENT)");
		printf("[%u] %s\n", i, kbs.kb_string);
	}
}

static void set(int fd)
{
	ioctl(fd, KDSKBSENT);
}

int main(int argc, char **argv)
{
	int fd = open(argv[1], O_RDWR);
	if (fd < 0)
		err(1, "open");

	get(fd);
	set(fd);

	close(fd);
	return 0;
}

