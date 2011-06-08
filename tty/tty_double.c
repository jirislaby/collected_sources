#include <err.h>
#include <fcntl.h>
#include <sched.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

static void wr(const char *tty, const char *text, unsigned int len)
{
	int fd, a;

	fd = open(tty, O_RDWR);
	if (fd < 0)
		err(2, "open");

	for (a = 0; a < 10; a++) {
		if (a == 4)
			if (sched_yield())
				err(2, "sched_yield");
		if (write(fd, text, len) != len)
			err(2, "write");
	}

	close(fd);
}

static void work(const char *tty, char *text, unsigned int len)
{
	while (1) {
		text[len - 3]++;
		text[len - 3] &= 0x7f;
		wr(tty, text, len);
	}
}

int main(int argc, char **argv)
{
	switch (fork()) {
	case -1:
		err(1, "fork");
	default: {
		static char text[] = "aaaaaaaaaaaaa\n";
		sprintf(text, "%d", getpid());
		work(argv[1], text, sizeof(text));
	}
	}

	return 0;
}
