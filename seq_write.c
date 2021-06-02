#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	unsigned int a = 0;

	while (1) {
		printf("%u\n", a++);
		if (!(a % 100)) {
			fprintf(stderr, "%u\n", a);
			usleep(rand() % 3000000);
		}
	}

	return 0;
}

