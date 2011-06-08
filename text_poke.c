#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAGIC 0x4343aa11deadcc22ULL

int main(int argc, char **argv)
{
	unsigned int a, b, c = 0;
	unsigned long long *ch;
	unsigned long size = atol(argv[1]) << 20;

	ch = malloc(size);
	if (!ch)
		err(1, "malloc");

	printf("alloced %luM: %p\n", size >> 20, ch);

	for (a = 0; a < size/sizeof(*ch); a++)
		ch[a] = MAGIC;

	puts("filled");

	while (1) {
		sleep(30);

		for (a = 0; a < size/sizeof(*ch); a++)
			if (ch[a] != MAGIC) {
				printf("WHAT THE HELL? iteration=%u off=%.8lx:\n",
						c, a * sizeof(*ch));
				for (b = a - a % 10; b < (a - a % 10) + 100; b++) {
					printf("%.16llx ", ch[b]);
					if (!((b + 1) % 10))
						puts("");
				}
				exit(1);
			}
		c++;
	}

	return 0;
}
