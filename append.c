#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    char ch[8];
    int fd=open("aaa", O_RDWR|O_APPEND);

    ch[7]=0;
    lseek(fd, -7, SEEK_END);
    printf("--%d, %s--", read(fd, ch, 7), ch);

    close(fd);

    return 0;
}
