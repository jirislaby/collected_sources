#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char **argv)
{
//	struct termios attr;
	int fd;

	if (!--argc)
		return 1;

	fd = open(*++argv, O_RDONLY);// | O_NONBLOCK);
	if (fd < 0) {
		perror("open");
		goto err;
	}

/*	if (tcgetattr(fd, &attr)) {
		perror("tcgetattr");
		goto errcl;
	}
	attr.c_cflag = B4800 | CS8 | CREAD;
	attr.c_iflag = 0;
	attr.c_oflag = 0;
	attr.c_lflag = 0;
	attr.c_cc[VMIN] = 12;
	attr.c_cc[VTIME] = 0;
	if (tcsetattr(fd, TCSANOW, &attr)) {
		perror("tcsetattr");
		goto errcl;
	}*/

	int a;
	if (ioctl(fd, TIOCMGET, &a) < 0) {
		perror("ioctl");
		goto errcl;
	}
	a |= TIOCM_RTS | TIOCM_CTS;
	a &= ~TIOCM_LE;
	if (ioctl(fd, TIOCMSET, &a) < 0) {
		perror("ioctl");
		goto errcl;
	}

	while (ioctl(fd, TIOCMGET, &a) >= 0) {
		printf("%x", !!(a & TIOCM_LE));
		fflush(stdout);
		usleep(1000);
	}

/*	char buf[32];
	unsigned int b;
	ssize_t rd;
	while ((rd = read(fd, buf, sizeof(buf))) > 0) {
		for (b = 0; b < rd; b++)
			printf("%x ", buf[b]);
	}*/
	puts("");

/*	if (rd < 0)
		perror("read");*/
	
	close(fd);

	return 0;
errcl:
	close(fd);
err:
	return 20;
}
