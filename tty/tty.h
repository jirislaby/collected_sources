#ifndef TTY_H
#define TTY_H

#include <err.h>
#include <errno.h>

static inline void warn_on_EPERM(const char *msg)
{
	if (errno == -EPERM)
		warn(msg);
}

#endif
