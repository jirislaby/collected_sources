#include <err.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{
	const unsigned samples = 48000 * sizeof(int16_t) * 2;
	int16_t x[2];
	ssize_t rd;
	off_t off = 0;

	while ((rd = read(0, &x, sizeof(x))) > 0) {
		double secs = (double)off / samples;
		printf("%8.5f %5zu %7d %7d %4x %4x\n",
		       secs, off, x[0], x[1], (uint16_t)x[0], (uint16_t)x[1]);
		off += rd;
	}

	return 0;
}

