#define _GNU_SOURCE
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/syscall.h>
#include <sys/types.h>

int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s filename\n", argv[0]);
		return 1;
	}

	if (syscall(299, AT_FDCWD, argv[1]))
		err(2, "revokeat");

	return 0;
}
