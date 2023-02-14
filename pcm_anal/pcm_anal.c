#include <err.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	unsigned long tot_count = 0;
	unsigned chans = 1, rate = 8000, div = 2, wmark = 30000;
	uint16_t max = 0, *vals;
	int opt;

	while ((opt = getopt(argc, argv, "c:r:")) >= 0) {
		switch (opt) {
		case 'c':
			chans = atoi(optarg);
			break;
		case 'r':
			rate = atoi(optarg);
			break;
		}
	}

	ssize_t to_read = chans * rate / div * sizeof(*vals);
	vals = malloc(to_read);

	while (1) {
		ssize_t rd = read(STDIN_FILENO, vals, to_read);
		if (rd < 0)
			err(1, "read");
		if (rd != to_read)
			break;

		for (unsigned i = 0; i < rate / div; i++)
			if (vals[i * chans] > max)
				max = vals[i * chans];

		if (max < 0x8000)
			printf("MAX %u\n", max);

		if (abs((int)max - 0x8000) > wmark) {
			unsigned long time = tot_count / div;
			printf("%3lu:%.2lu:%.2lu.%.2lu %6d %5u ",
			       time / 3600, (time / 60) % 60,
			       time % 60, (time * 10000) % 100,
			       (int)max - 0x8000, max);
			for (unsigned i = 0; i < max / (USHRT_MAX / 150); i++)
				putchar('-');
			puts("");
		}

		max = 0;
		tot_count++;
	}

	return 0;
}

