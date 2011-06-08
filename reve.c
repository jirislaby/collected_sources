#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int fd=open("soutez-3.bin", O_RDONLY);

    int length=lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    
    char *ptr=mmap(NULL, length, PROT_READ, MAP_SHARED, fd, 0);

    int fd1=open("out",O_WRONLY|O_CREAT);
    char *ptr1 = ptr + length-1;
    for (; ptr1>=ptr; ptr1--)
	write(fd1, ptr1, 1);

    close(fd1);
    munmap(ptr, length);

    close(fd);
    return 0;
}
