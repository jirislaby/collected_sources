#include <stdio.h>

int main()
{
	printf("%d %d\n", __alignof__(unsigned long long), __alignof__(void *));

	return 0;
}
