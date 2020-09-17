#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
	unsigned int i;

	while (1) {
		for (i = 0; i < rand() % 64 + 64; i++)
			putchar('X');
		printf("\e[0G\e[%uP\n", rand() % 128);
		//printf("\e[0G\e[2P\n");
	}

	return 0;
}

