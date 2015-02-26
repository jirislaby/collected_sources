#include <err.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static void dump_file(unsigned long from, unsigned long to)
{
	printf("\n===\n%lx-%lx\n", from, to);

	int fd = open("bin", O_WRONLY|O_CREAT|O_APPEND, 0755);
	if (fd < 0) {
		warn("open w");
		return;
	}
	write(fd, (void *)from, to - from);
	close(fd);
}

static void pr(int fd)
{
	char buf[10000], *p = buf;
	ssize_t sz;
	unsigned long from, to;

	sz = read(fd, buf, sizeof(buf));
	if (sz < 0) {
		warn("read");
		return;
	}

	if (write(1, buf, sz) != sz)
		warn("write");

	if (sscanf(p, "%lx-%lx", &from, &to) != 2) {
		warn("sscanf");
		return;
	}
	while (*p != '\n')
		p++;
	p++;

	dump_file(from, to);

	if (sscanf(p, "%lx-%lx", &from, &to) != 2) {
		warn("sscanf");
		return;
	}
	while (*p != '\n')
		p++;
	p++;
//	dump_file(from, to);

	if (sscanf(p, "%lx-%lx", &from, &to) != 2) {
		warn("sscanf");
		return;
	}
	while (*p != '\n')
		p++;
//	dump_file(from, to);

}

static void sig_h(int s)
{
	int fd = open("/proc/self/maps", O_RDONLY);

	printf("ahoj %p\n", pr);
	if (fd < 0) {
		warn("open");
		return;
	}

	pr(fd);

	close(fd);
}

static void __attribute__ ((constructor)) read_init(void)
{
/*	signal(SIGALRM, sig_h);
	alarm(1);*/
	sig_h(SIGALRM);
}

int __attribute__ ((weak)) main()
{
	sig_h(SIGALRM);
	return 0;
}
