#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char **argv)
{
	char buf[128];
	int fd;
    while (1) {
        fd = open("/dev/ttyS1", O_RDWR);
        if (fd < 0) {
                warn("open");
                continue;
        }
	if (read(fd, buf, sizeof(buf)) < 0)
                warn("read");
        close(fd);
    }
    return 0;
}
