#define _GNU_SOURCE
#include <err.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>

static const char file[] = "/sys/kernel/notes";

void child(void)
{
	struct stat stat;
	int fd, fd1;
	char c;

	fd = open(file, O_RDWR);
	if (fd < 0)
		err(1000, "open");

	while (1) {
		if (fstat(fd, &stat) < 0)
			warn("fstat");
		if (read(fd, &c, 1) < 0)
			warn("read");
		fd1 = open(file, O_RDWR);
		if (fd1 < 0)
			warn("open1");
		else
			close(fd1);
		sleep(1);
	}
}

int main(void)
{
	int ret;

	switch (fork()) {
	case 0:
		child();
	case -1:
		err(1, "fork");
	default:
		break;
	}

	sleep(5);
	if (syscall(299, 0, file))
		warn("revoke");

	wait(&ret);

	return ret;
}
