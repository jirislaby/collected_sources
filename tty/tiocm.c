#include <err.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define TIO_BITS(op)	\
	op(LE),		\
	op(DTR),	\
	op(RTS),	\
	op(ST),		\
	op(SR),		\
	op(CTS),	\
	op(CAR),	\
	op(RNG),	\
	op(DSR)

static struct {
	const char *name;
	unsigned int bit;
} names[] = {
#define MKNAM(a) { #a, TIOCM_ ## a }
	TIO_BITS(MKNAM)
#undef MKNAM
};

static void dump_tiocm(int tio)
{
	int a;

	printf("TIOCM: %.8x\n", tio);
	for (a = 0; a < sizeof(names)/sizeof(*names); a++)
		if (names[a].bit & tio)
			printf("  %s\n", names[a].name);
}

static void usage(const char *program)
{
	errx(100, "%s /dev/ttyS0", program);
}

int main(int argc, char **argv)
{
	static int opt_tiocm;
	static const struct option opts[] = {
#define MKOPT(a) { #a "+", 0, &opt_tiocm, TIOCM_ ## a }
		TIO_BITS(MKOPT),
#undef MKOPT
#define MKOPT(a) { #a "-", 0, &opt_tiocm, TIOCM_ ## a }
		TIO_BITS(MKOPT),
#undef MKOPT
#define MKOPT(a) { #a "*", 0, &opt_tiocm, TIOCM_ ## a }
		TIO_BITS(MKOPT),
#undef MKOPT
		{ "sleep", 1, NULL, 's' },
		{}
	};
	const char *tty;
	int fd, tio, orig_tio, o, o_idx;
	int clear = 0, set = 0, alter = 0;
	unsigned int sleep = 0;

	while ((o = getopt_long(argc, argv, "s:", opts, &o_idx)) >= 0) {
		switch (o) {
		case 0: {
			const char *name = opts[o_idx].name;
			const char last = name[strlen(name) - 1];

			if (last == '-')
				clear |= opts[o_idx].val;
			else if (last == '+')
				set |= opts[o_idx].val;
			else
				alter |= opts[o_idx].val;
			break;
		}
		case 's':
			sleep = atoi(optarg);
			break;
		default:
			usage(argv[0]);
		}
	}

	if (argc - optind < 1)
		usage(argv[0]);

	tty = argv[optind];

	fd = open(tty, O_RDWR);
	if (fd < 0)
		err(1, "open");

	puts("Original value:");
	if (ioctl(fd, TIOCMGET, &tio))
		err(2, "ioctl(TIOCMGET)");

	orig_tio = tio;

	dump_tiocm(tio);

	tio |= set;
	tio &= ~clear;

	if (tio != orig_tio || alter) {
		do {
			puts("Changing to:");
			dump_tiocm(tio);

			if (ioctl(fd, TIOCMSET, &tio))
				err(2, "ioctl(TIOCMSET)");

			puts("Changed to:");
			if (ioctl(fd, TIOCMGET, &tio))
				err(2, "ioctl(TIOCMGET)");

			dump_tiocm(tio);

			if (sleep) {
				printf("sleeping %u ms\n", sleep);
				usleep(sleep * 1000);
			}

			tio ^= alter;
		} while (alter);
	}

	close(fd);

	return 0;
}
