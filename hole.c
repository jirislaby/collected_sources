#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
    int fd = open("aaa", O_WRONLY|O_CREAT, 0664);

    write(fd, "a", 1);

    lseek(fd, 1<<20, SEEK_SET);

    write(fd, "b", 1);

    close(fd);

    return 0;
}
