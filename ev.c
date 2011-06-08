#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <asm/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#include <linux/input.h>

int main(void)
{
	struct input_event ev[64];
	ssize_t read_bytes;
	unsigned int loop, yalv;
	int fd;

	puts("tu");
	while (1) {
		fd = open("/dev/input/event15", O_RDONLY);
		if (fd >= 0)
			break;
		perror("open");
	}
	puts("tu1");

	for (loop = 0; loop < 100; loop++) {
		read_bytes = read(fd, ev, sizeof(struct input_event) * 64);

		if (read_bytes < (ssize_t)sizeof(struct input_event)) {
			perror("evtest: short read");
			exit(1);
		}

		for (yalv = 0; yalv < (read_bytes / sizeof(struct input_event)); yalv++)
			printf("Event: time %ld.%06ld, type %d, code %d, value %d\n",
				ev[yalv].time.tv_sec, ev[yalv].time.tv_usec, ev[yalv].type,
				ev[yalv].code, ev[yalv].value);
	}

	close(fd);

	return 0;
}
