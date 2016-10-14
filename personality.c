#include <err.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/personality.h>

int main(int argc, char **argv)
{
	char *ptr = NULL;

	int pers = personality(MMAP_PAGE_ZERO);
	if (pers < 0)
		err(1, "personality");

	if (pers & MMAP_PAGE_ZERO) {
		printf("%d\n", ptr[4095]);
		return 0;
	}

	execv(argv[0], argv);

	return 0;
}
