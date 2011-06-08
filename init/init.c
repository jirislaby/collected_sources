#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/mount.h>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <sys/swap.h>
#include <sys/types.h>
#include <sys/utsname.h>

static void test_sleep(const char *type)
{
	int fd;

	puts("mounting /sys");

	if (mount("none", "/sys", "sysfs", 0, "")) {
		perror("mount /sys failed");
		return;
	}
	fd = open("/sys/power/state", O_RDWR);
	if (fd < 0) {
		perror("open /sys/power/state failed");
		goto err_um;
	}

	printf("going to sleep: %s\n", type);
	fsync(STDOUT_FILENO);
	fsync(STDERR_FILENO);

	if (write(fd, type, strlen(type)) != strlen(type))
		perror("write unsuccessfull");

	close(fd);

err_um:
	if (umount("/sys"))
		perror("umount /sys failed");
}

int main(void)
{
	struct utsname un;

	if (uname(&un))
		perror("uname failed");

	printf("\nahoj (%s built: %s)\n\n", un.release, un.version);

	if (swapon("/dev/sdb", 0))
		perror("swapon sdb failed");

	test_sleep("disk");

	if (swapoff("/dev/sdb"))
		perror("swapoff sdb failed");

	if (umount("/"))
		perror("umount / failed");

	sync();

	puts("");

	sleep(10);

	if (!fork())
		reboot(RB_HALT_SYSTEM);

	while (1)
		sleep(10);

	return 0;
}
