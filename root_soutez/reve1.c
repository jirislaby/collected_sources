#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>

int main()
{
    int length=lseek(0, 0, SEEK_END);
    lseek(0, 0, SEEK_SET);
    
    char *ptr  = mmap(NULL, length, PROT_READ, MAP_SHARED, 0, 0);
    char *ptr1 = ptr + length-1;

    for (; ptr1>=ptr; ptr1--)
	write(1, ptr1, 1);

    munmap(ptr, length);

    return 0;
}
