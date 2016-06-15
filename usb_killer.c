#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#define DEF_SIZE 0x800000

int main(int argc, char **argv)
{
	void *ptr;
	size_t size = DEF_SIZE;
	int fd;

	if (argc > 1)
		size = atol(argv[1]);

	fd = open("/dev/bus/usb/001/001", O_RDONLY);
	if (fd < 0)
		err(1, "open");

	printf("mapping %zu MB\n", size >> 20);
	ptr = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
	if (ptr == MAP_FAILED)
		err(1, "mmap");

	sleep(10);

	munmap(ptr, size);
	close(fd);

	return 0;
}
