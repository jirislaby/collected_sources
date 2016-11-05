#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

int main()
{
	int r0 = open("/dev/ptmx", 0x200);
	int val = 0x12;
	ioctl(r0, 0x5423, &val); // TIOCSETD

}
