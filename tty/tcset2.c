#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>

#include <asm/termbits.h>

static void set_speed(int tty, speed_t sp)
{
	struct termios2 t, t2;

	if (ioctl(tty, TCGETS2, &t))
		err(EXIT_FAILURE, "TCGETS2 1");

	t.c_ispeed = t.c_ospeed = sp;
	t.c_cflag &= ~CBAUD;
	t.c_cflag |= BOTHER;
	if (ioctl(tty, TCSETS2, &t))
		err(EXIT_FAILURE, "TCSETS2");

	if (ioctl(tty, TCGETS2, &t2))
		err(EXIT_FAILURE, "TCGETS2 2");

	if (t.c_ispeed != t2.c_ispeed)
		warnx("ispeed unset: wanted %u != set %u", t.c_ispeed, t2.c_ispeed);
	if (t.c_ospeed != t2.c_ospeed)
		warnx("ospeed unset: wanted %u != set %u", t.c_ospeed, t2.c_ospeed);
}


int main(int argc, char **argv)
{
	if (argc != 3)
		errx(EXIT_FAILURE, "bad args");

	const char *file = argv[1];
	speed_t sp = atoi(argv[2]);

	int tty = open(file, O_RDWR);
	if (tty < 0)
		err(EXIT_FAILURE, "open %s", file);

	printf("setting %s speed to %u\n", file, sp);

	set_speed(tty, sp);

	close(tty);

	return 0;
}

