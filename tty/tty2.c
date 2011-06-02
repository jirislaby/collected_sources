#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(void)
{
   int fd, ld1, ld2;
    while (1) {
            fd = open("/dev/ttyS1", O_RDWR);
            ld1 = 0; ld2 = 2;
            while (1) {
                    ioctl(fd, TIOCSETD, &ld1);
                    ioctl(fd, TIOCSETD, &ld2);
            }
            close(fd);
    }
}
