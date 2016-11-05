#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <linux/tty.h>

int main()
{
	int r0 = open("/dev/ptmx", O_RDONLY);
	int val = N_PPS;
	ioctl(r0, TIOCSETD, &val); // TIOCSETD

}
