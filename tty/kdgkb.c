#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <linux/kd.h>
#include <linux/keyboard.h>

static char *func[MAX_NR_FUNC];

static void get(int fd)
{
	unsigned i, j;

	for (i = 0; i < MAX_NR_FUNC; i++) {
		struct kbsentry kbs = {
			.kb_func = i,
			.kb_string = "X",
		};
		if (ioctl(fd, KDGKBSENT, &kbs) < 0)
			err(1, "ioctl(KDGKBSENT) [%u]", i);
		func[i] = strdup((char *)kbs.kb_string);
		printf("[%u] ", i);
		for (j = 0; j < strlen((char *)kbs.kb_string); j++) {
			const char ch = kbs.kb_string[j];
			putchar(isprint(ch) ? ch : '.');
		}
		puts("");
	}
}

static void set(int fd)
{
	unsigned i;

	for (i = 0; i < MAX_NR_FUNC; i++) {
		struct kbsentry kbs = {
			.kb_func = i,
		};
		if (!strlen(func[i]))
			continue;
		strcpy((char *)kbs.kb_string, func[i]);

		if (ioctl(fd, KDSKBSENT, &kbs) < 0)
			err(1, "ioctl(KDSKBSENT) [%u]", i);
	}

	for (i = 0; i < MAX_NR_FUNC; i++) {
		struct kbsentry kbs = {
			.kb_func = i,
		};
		char *from = func[i];
		if (!strlen(from))
			from = "AHOJ";
		snprintf((char *)kbs.kb_string, sizeof(kbs.kb_string), "%s%s", from, from);
		kbs.kb_string[sizeof(kbs.kb_string) - 1] = 0;

		if (ioctl(fd, KDSKBSENT, &kbs) < 0)
			err(1, "ioctl(KDSKBSENT) [%u]", i);
	}
}

static void alter(int fd)
{
	struct kbsentry kbs = {
		.kb_func = 0,
		.kb_string = "XXXXXXXXXXXXXXXXXXX\n",
	};
	char buf[64];
	unsigned a;
	int fl;

	if (ioctl(fd, KDSKBSENT, &kbs) < 0)
		err(1, "ioctl(KDSKBSENT) initial");

	fl = fcntl(0, F_GETFL);
	if (fl < 0)
		err(1, "fcntl(F_GETFL)");
	printf("stdin flags=%x -> %x\n", fl, fl | O_NONBLOCK);
	fl |= O_NONBLOCK;
	if (fcntl(0, F_SETFL, fl) < 0)
		err(1, "fcntl(F_SETFL)");

	while (1) {
		for (a = 0; a < 2; a++) {
			strcpy((char *)kbs.kb_string, a ? ".\n" : "88888888888888888888\n");
			if (ioctl(fd, KDSKBSENT, &kbs) < 0)
				err(1, "ioctl(KDSKBSENT) [%u]", a);
		}
		ssize_t rd = read(0, buf, sizeof(buf));
		if (rd < 0 && errno != EAGAIN)
			err(1, "read");

	}
}

int main(int /*argc*/, char **argv)
{
	int fd = open(argv[1] ? : "/dev/tty", O_RDWR);
	if (fd < 0)
		err(1, "open");

	alter(fd);

	get(fd);
	set(fd);
	get(fd);

	close(fd);
	return 0;
}

