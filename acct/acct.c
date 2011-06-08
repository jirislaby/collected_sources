#define _GNU_SOURCE
#include <err.h>
#include <fcntl.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define XXX 10

static unsigned short arr[XXX];

static void *thread(void *unused)
{
	unsigned long x = (unsigned long)unused;
	unsigned int counter = 0;
	struct rlimit r;

	if (!x)
		printf("%.20s", " ");
	while (1) {
		if (getrlimit(RLIMIT_CPU, &r)) {
			warn("getrlimit");
			break;
		}
		if (r.rlim_cur == RLIM_INFINITY) {
			arr[x]++;
//			puts("broken");
//			break;
		}
		if (!x && !((++counter) % 10000)) {
			unsigned int a;
/*			for (a = 0; a < XXX * 5; a++)
				putchar('\b');*/
			for (a = 0; a < XXX; a++)
				printf("%.4x ", arr[a]);
			puts("");
		}
	}
	return 0;
}

#define STACK_SIZE (10*4096)

int main(int argc, char **argv)
{
	pthread_t thr[XXX];
	unsigned int a;
	int fd;
/*	int ch;
	void *stack;

	stack = mmap(NULL, 2*STACK_SIZE, PROT_READ|PROT_WRITE,
			MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	if (stack == MAP_FAILED)
		err(5, "mmap");

	ch = clone(thread, stack + STACK_SIZE, CLONE_THREAD | SIGCHLD |
				CLONE_SIGHAND | CLONE_VM,
				(void *)(unsigned long)0);
	if (ch < 0)
		err(2, "clone");*/
	for (a = 0; a < XXX; a++)
		if (pthread_create(&thr[a], NULL, thread, (void *)(unsigned long)a))
			errx(5, "pthread_create");

	sleep(1);

	fd = open("/dev/my", O_RDWR);
	if (fd < 0)
		err(1, "open");

	if (ioctl(fd, 0))
		err(2, "ioctl");

	close(fd);

	puts("");
	puts("");
	for (a = 0; a < XXX; a++)
		printf("%.4x ", arr[a]);
	puts("");
	sleep(2);

	return 0;
}
