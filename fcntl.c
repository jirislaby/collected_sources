#include <err.h>
#include <fcntl.h>

#define F_SETPIPE_SZ (1024 + 7)

int main()
{
	if (fcntl(0, F_SETOWN, 0x800000000) < 0)
		warn("F_SETOWN 1");
	if (fcntl(0, F_SETOWN, 0x80000000) < 0)
		warn("F_SETOWN 2");
	if (fcntl(0, F_SETPIPE_SZ, 0) < 0)
		warn("F_SETPIPE_SZ 2");
	return 0;
}
