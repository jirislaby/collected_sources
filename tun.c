#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <linux/if.h>
#include <linux/if_tun.h>

int main()
{
	struct ifreq ifreq = {
		.ifr_ifrn.ifrn_name = "kill",
		.ifr_ifru.ifru_flags = IFF_NO_PI | IFF_VNET_HDR | IFF_TUN,
	};
	char blank[10] = {};
	int r0;

	r0 = open("/dev/net/tun", O_RDWR);
	if (r0 < 0)
		err(1, "open");

	if (ioctl(r0, TUNSETIFF, &ifreq) < 0)
		err(1, "ioctl");

	system("ip l set dev kill up");

	if (write(r0, blank, sizeof(blank)) < 0)
		err(1, "write");

	close(r0);

	return 0;
}
