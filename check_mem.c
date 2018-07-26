#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/mman.h>

static const unsigned char pattern = 0x8e;
static const unsigned int context = 100;

static void dump(unsigned char *mem, unsigned long size, unsigned long a)
{
	unsigned long b;

	printf("mem=%p, a=%lu, mem[a]=%x\n", mem, a, mem[a]);
	if (a < context)
		b = 0;
	else
		b = a - context;
	b = b & ~0xf;
	for (; b < size && b < a + context; b++) {
		printf("%c%.2x", a == b ? '>' : ' ', mem[b]);
		if (!((b+1) % 16))
			puts("");
	}
	puts("");
}

int main(int argc, char **argv)
{
	unsigned long a, size = atol(argv[1]) << 20;
	unsigned char *mem;
	bool term = false;

	mem = mmap(NULL, size, PROT_READ|PROT_WRITE,
			MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	if (mem == MAP_FAILED)
		err(1, "mmap");

	printf("writing pattern...");
	fflush(stdout);
	memset(mem, pattern, size);
	printf(" DONE\n");

	while (1) {
		printf("checking...");
		fflush(stdout);

		for (a = 0; a < size; a++)
			if (mem[a] != pattern) {
				term = true;
				break;
			}

		if (term) {
			puts("");
			break;
		}

		puts(" DONE");

		sleep(30);
	}

	dump(mem, size, a);

	munmap(mem, size);

	return 0;
}
