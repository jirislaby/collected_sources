#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MKNAM(a) { #a, TIOCM_ ## a }
static struct {
	const char *name;
	unsigned int bit;
} names[] = {
	MKNAM(LE),
	MKNAM(DTR),
	MKNAM(RTS),
	MKNAM(ST),
	MKNAM(SR),
	MKNAM(CTS),
	MKNAM(CAR),
	MKNAM(RNG),
	MKNAM(DSR),
};

static void dump_tiocm(int tio)
{
	int a;

	printf("TIOCM: %.8x\n", tio);
	for (a = 0; a < sizeof(names)/sizeof(*names); a++)
		if (names[a].bit & tio)
			printf("  %s\n", names[a].name);
}

static int get_tio_bit(const char *bit)
{
	int a;

	for (a = 0; a < sizeof(names)/sizeof(*names); a++)
		if (!strcmp(names[a].name, bit))
			return names[a].bit;
	return 0;
}

int main(int argc, char **argv)
{
	const char *tty;
	int fd, tio, orig_tio;

	if (argc < 2)
		errx(100, "%s /dev/ttyS0", argv[0]);

	tty = argv[1];
	argv += 2;
	argc -= 2;

	fd = open(tty, O_RDWR);
	if (fd < 0)
		err(1, "open");

	puts("Original value:");
	if (ioctl(fd, TIOCMGET, &tio))
		err(2, "ioctl(TIOCMGET)");

	orig_tio = tio;

	dump_tiocm(tio);

	while (argc--) {
		const char *val = *argv;
		int bit;
		if (*val != '+' && *val != '-') {
			warnx("ignoring '%s'", val);
			continue;
		}

		bit = get_tio_bit(val + 1);
		if (!bit) {
			warnx("ignoring '%s'", val);
			continue;
		}

		if (*val == '+')
			tio |= bit;
		else
			tio &= ~bit;
		argv++;
	}

	if (tio != orig_tio) {
		puts("Changing to:");
		dump_tiocm(tio);

		if (ioctl(fd, TIOCMSET, &tio))
			err(2, "ioctl(TIOCMSET)");

		puts("Changed to:");
		if (ioctl(fd, TIOCMGET, &tio))
			err(2, "ioctl(TIOCMGET)");

		dump_tiocm(tio);
	}

	close(fd);

	return 0;
}
