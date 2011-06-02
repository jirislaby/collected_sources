#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/input.h>

int main(int argc, char** argv)
{
	struct ff_effect eff;
	struct input_event ev;
	unsigned int a;
	int fd, ids[2];
	char *dev = "/dev/input/momo";

	if (argc > 1)
		dev = argv[1];

	fd = open(dev, O_RDWR);
	if (fd < 0) {
		perror("Open device file");
		exit(1);
	}

	memset(&eff, 0, sizeof(eff));
#if 1
	eff.type = FF_CONSTANT;
	eff.id = -1;
	eff.u.constant.level = 0x3000;
	eff.direction = 0;
	eff.replay.length = 0x7fff;

	if (ioctl(fd, EVIOCSFF, &eff) == -1) {
		perror("Upload eff");
		exit(1);
	}
	ids[0] = eff.id;

	eff.id = -1;
	eff.direction = 0x7fff;
	if (ioctl(fd, EVIOCSFF, &eff) == -1) {
		perror("Upload eff");
		exit(1);
	}
	ids[1] = eff.id;
#else
	eff.type = FF_RUMBLE;
	eff.id = -1;
	eff.u.rumble.strong_magnitude = 0x8000;
	eff.u.rumble.weak_magnitude = 0;
	eff.replay.length = 5000;
	if (ioctl(fd, EVIOCSFF, &eff) == -1) {
		perror("Upload eff");
		exit(1);
	}
	ids[0] = eff.id;

	eff.id = -1;
	eff.u.rumble.strong_magnitude = 0;
	eff.u.rumble.weak_magnitude = 0x8000;
	if (ioctl(fd, EVIOCSFF, &eff) == -1) {
		perror("Upload eff");
		exit(1);
	}
	ids[1] = eff.id;
#endif

	ev.type = EV_FF;

	for (a = 0; a < 200; a++) {
		ev.code = ids[a % 2];
		ev.value = 1;
		if (write(fd, &ev, sizeof(ev)) == -1) {
			perror("Play effect");
			exit(1);
		}
		usleep(10000);
		ev.value = 0;
		if (write(fd, &ev, sizeof(ev)) == -1) {
			perror("Play effect");
			exit(1);
		}
	}

	return 0;
}
