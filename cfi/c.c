#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>

extern void asm_fun(void);

static void dump_stack()
{
	void *funs[20];
	char **funn;
	int count, i;

	count = backtrace(funs, sizeof(funs)/sizeof(*funs));

	funn = backtrace_symbols(funs, count);
	for (i = 0; i < count; i++)
		printf("%2d: %p -- %s\n", i, funs[i], funn[i]);
	free(funn);
}

void c_fun(void)
{
	puts("ahoj");
	dump_stack();
}

int main(void)
{
	asm_fun();
	puts("=======");
	dump_stack();
	return 0;
}
