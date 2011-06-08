#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 1000000

int main()
{
	unsigned int a, b = 0, c, d, e;

	srand((unsigned int)time(NULL));

	for (a = 0; a < MAX; a++) {
		c = rand() % 6;
		d = rand() % 6;
		e = rand() % 6;
		if ((c == d && c != e) || (c == e && c != d) ||
				(d == e && c != e))
			b++;
	}

	printf("prob=%u/%u=%.5f\n", b, MAX, (float)b / MAX);

	return 0;
}
