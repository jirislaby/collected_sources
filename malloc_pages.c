#include <stdio.h>
#include <stdlib.h>

#define SIZE (1800 << 20)

int main()
{
	char *p = malloc(SIZE);
	unsigned int x;

	for (x = 0; x < SIZE; x += 4096)
		p[x] = 10;

	free(p);

	return 0;
}
