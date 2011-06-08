#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

int main()
{
	struct termios t;
	int count, a;
	char buf[12];
	int fd = open("/dev/ttyUSB0", O_RDWR);

	if (fd < 0) {
		perror("open");
		return 1;
	}

	if (tcgetattr(fd, &t)) {
		perror("geta");
		return 2;
	}
	t.c_cflag = B4800 | CS8 | CREAD;
	t.c_iflag = 0;
	t.c_oflag = 0;
	t.c_lflag = 0;
	t.c_cc[VMIN] = 12;
	t.c_cc[VTIME] = 0;
	if (tcflush(fd, TCIFLUSH)) {
		perror("flush");
		return 2;
	}
	if (tcsetattr(fd, TCSANOW, &t)) {
		perror("seta");
		return 2;
	}
	buf[0] = 0;
	while (buf[0] != 0x0a)
		read(fd, buf, 1);

	while ((count = read(fd, buf, sizeof(buf)))) {
		if (count < 0) {
			perror("read");
			return 3;
		}

		if (count != sizeof(buf))
			continue;
		printf("%10d, %s", rand(), buf);
	}

	close(fd);
}
