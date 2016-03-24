#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <linux/tty.h>

#include "gsmmux.h"

#define DEFAULT_SPEED	B115200

int main(int argc, char **argv)
{
	struct gsm_config c;
	int ldisc = N_GSM0710;
	int fd;

	fd = open(argv[1], O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd < 0)
		err(1, "open");

	write(fd, "AT+CMUX=0\r", 10);

	usleep(300000);

	if (ioctl(fd, TIOCSETD, &ldisc) < 0)
		err(1, "ioctl(TIOCSETD)");

	/* get n_gsm configuration */
	if (ioctl(fd, GSMIOC_GETCONF, &c) < 0)
		err(1, "ioctl(GSMIOC_GETCONF)");
	/* we are initiator and need encoding 0 (basic) */
	c.initiator = 1;
	c.encapsulation = 0;
	/* our modem defaults to a maximum size of 127 bytes */
	c.mru = 127;
	c.mtu = 127;
	/* set the new configuration */
	ioctl(fd, GSMIOC_SETCONF, &c);

	pause();

	return 0;
}
