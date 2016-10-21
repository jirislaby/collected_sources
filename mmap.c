#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

int main()
{
	unsigned char *m;
	int a, fd = open("soubor.txt", O_RDWR | O_CREAT, 0644);
	ftruncate(fd, 1 << 20);
	m = mmap(NULL, 1 << 20, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	for (a = 0; a < 1 << 20; a++) {
		if (!(a%16))
			puts("");
		printf(" %.2x", m[a]);
	}
	puts("");
	strcpy(&m[500 << 10], "ahoj");
	return 0;
}
