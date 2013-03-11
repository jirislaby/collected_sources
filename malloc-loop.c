#include <stdlib.h>

int main(void)
{
	void *ptr1;
	void **ptr = &ptr1;
	while (1) {
		*ptr = malloc(1 << 20);
		ptr = *ptr;
		asm volatile("" : : : "memory");
	}
	return 0;
}
