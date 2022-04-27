#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/wait.h>

#include <termio.h>

#include "process.h"

// # {Threaded:true Repeat:true RepeatTimes:0 Procs:8 Slowdown:1 Sandbox:none
// Leak:false NetInjection:false NetDevices:true NetReset:true Cgroups:true
// BinfmtMisc:true CloseFDs:true KCSAN:false DevlinkPCI:false USB:false
// VhciInjection:false Wifi:false IEEE802154:false Sysctl:true UseTmpDir:true
// HandleSegv:true Repro:false Trace:false LegacyOptions:{Collide:false Fault:false
// FaultCall:0 FaultNth:0}}

struct handle {
	int master;
	int slave;
};

static void proc_STI(void *arg)
{
	struct handle *h = arg;
	char c = 8;

	if (ioctl(h->slave, TIOCSTI, &c) < 0) //  (async, rerun: 64)
		err(1, "ioctl(TIOCSTI)");
}

static void proc_TCXONC(void *arg)
{
	struct handle *h = arg;

	if (ioctl(h->slave, TCXONC, TCIOFF) < 0) // (async, rerun: 64)
		err(1, "ioctl(TCXONC)");
}

static void proc_write(void *arg)
{
	static const char buf[] = "xxxxxxxxxxxxxxxxxx";
	struct handle *h = arg;

	if (write(h->slave, buf, strlen(buf)) < 0)
		err(1, "write");
}

static void post_fork(void *arg)
{
	struct handle *h = arg;

	close(h->master);
}

static void threads_done(void *arg)
{
	struct handle *h = arg;

	close(h->slave);
}

static const struct th_hooks hooks_STI = {
	.post_fork = post_fork,
	.threads_done = threads_done,
	.thread = proc_STI,
};

static const struct th_hooks hooks_TCXONC = {
	.post_fork = post_fork,
	.threads_done = threads_done,
	.thread = proc_TCXONC,
};

static const struct th_hooks hooks_write = {
	.post_fork = post_fork,
	.threads_done = threads_done,
	.thread = proc_write,
};

static void proc_waste(void *)
{
	unsigned int a;

	for (a = 0; a < 1000; a++)
		asm volatile("" ::: "memory");
}

static const struct th_hooks hooks_waste = {
	.thread = proc_waste,
};

static inline void dump_read_stats(size_t stat[6], unsigned int procs,
		unsigned char last_unk)
{
	printf("PARENT: read %zu (%zu/%zu/%zu/%zu/%zu - %.2x), waiting for %u procs\n",
			stat[0], stat[1], stat[2], stat[3], stat[4], stat[5],
			last_unk, procs);
}

static unsigned int read_master(int master, unsigned int procs)
{
	unsigned int i, read_loop = 0;
	size_t stat[6] = {};
	char buf[64], last_unk = 0;
	pid_t pid;

	while (procs) {
		ssize_t rd = read(master, buf, sizeof(buf));
		if (rd <= 0)
			break;
		stat[0] += rd;

		for (i = 0; i < rd; i++)
			if (buf[i] == 0x5e)
				continue;
			else if (buf[i] == 0x00)
				stat[1]++;
			else if (buf[i] == 0x48)
				stat[2]++;
			else if (buf[i] == 0x13)
				stat[3]++;
			else if (buf[i] == 'x')
				stat[4]++;
			else {
				stat[5]++;
				last_unk = buf[i];
			}

		if (!(++read_loop % 10000)) {
			unsigned int delay = rand() % 100000;
			if (delay > 1000)
				usleep(delay);

			dump_read_stats(stat, procs, last_unk);
		}

		pid = waitpid(-1, NULL, WNOHANG);
		if (pid > 0) {
			printf("PARENT: reaped %u\n", pid);
			procs--;
		}
	}

	dump_read_stats(stat, procs, last_unk);

	return procs;
}

#define PROCS 2
#define THREADS 2
#define LOOPS 10000000

int main(/*int argc, char **argv*/)
{
	struct handle h;
	unsigned int procs;
	int lck = 0;

	printf("STI:    %p\n", proc_STI);
	printf("TCXONC: %p\n", proc_TCXONC);
	printf("write:  %p\n", proc_write);
	fflush(stdout);

	h.master = open("/dev/ptmx", O_RDONLY);
	if (h.master < 0)
		err(1, "open");

	if (ioctl(h.master, TIOCSPTLCK, &lck) < 0)
		err(1, "ioctl(TIOCSPTLCK)");

	h.slave = ioctl(h.master, TIOCGPTPEER, O_RDWR);
	if (h.slave < 0)
		err(1, "ioctl(TIOCGPTPEER)");

	procs = run_parallel(PROCS, THREADS, LOOPS, &hooks_STI, &h);
	procs += run_parallel(PROCS, THREADS, LOOPS, &hooks_TCXONC, &h);
	if (0)
		procs += run_parallel(PROCS, THREADS, LOOPS, &hooks_write, &h);
	if (0)
		procs += run_parallel(1, 1, ~0U, &hooks_waste, NULL);

	close(h.slave);

	procs = read_master(h.master, procs);

	wait_for_parallel(procs);

	close(h.master);
	return 0;
}

