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


int main (int argc, char **argv) {

    int fd = -1;        /* the file descriptor for the device */
    int yalv;           /* loop counter */
    size_t read_bytes;  /* how many bytes were read */
    struct input_event ev[64]; /* the events (up to 64 at once) */

    /* read() requires a file descriptor, so we check for one, and then open it */
    if (argc != 2) {
	fprintf(stderr, "usage: %s event-device - probably /dev/input/evdev0\n", argv[0]);
	exit(1);
    }
    if ((fd = open(argv[1], O_RDONLY)) < 0) {
	perror("evdev open");
	exit(1);
    }

    while (1)
	{
	read_bytes = read(fd, ev, sizeof(struct input_event) * 64);

	if (read_bytes < (int) sizeof(struct input_event)) {
	    perror("evtest: short read");
	    exit (1);
	}

	for (yalv = 0; yalv < (int) (read_bytes / sizeof(struct input_event)); yalv++)
	    if (ev[yalv].type == 3)
	    {
		printf("Event: time %ld.%06ld, type %d, code %d, value %d\n",
		       ev[yalv].time.tv_sec, ev[yalv].time.tv_usec, ev[yalv].type,
		       ev[yalv].code, ev[yalv].value);

	    }
	}

    close(fd);

    exit(0);
}
