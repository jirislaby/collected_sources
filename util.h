#ifndef UTIL_H
#define UTIL_H

#include <err.h>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>

static inline void rand_sched()
{
	unsigned int rnd = rand();
	unsigned int ms =		rnd & 0x000003ff; // 1023
	unsigned int sleep_sched =	rnd & 0x00001000;

	if (!(rnd % 4)) {
		if (sleep_sched)
			usleep(1000 + 1000 * ms);
		else if (sched_yield())
			err(2, "sched_yield");
	}
}

#endif
