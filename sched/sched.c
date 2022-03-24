#include <err.h>
#include <errno.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const unsigned long loops = 0x200000000U;

int main(int argc, char **argv)
{
	struct sched_param param = {
		.sched_priority = 99,
	};
	unsigned long i;

	if (sched_setscheduler(0, SCHED_RR, &param))
		err(1, "sched_setscheduler");

	for (i = 0; i < loops; i++)
		asm volatile("" ::: "memory");

	return 0;
}

