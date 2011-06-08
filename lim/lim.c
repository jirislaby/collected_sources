#define _GNU_SOURCE

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/resource.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include <linux/types.h>

int getprlimit(pid_t pid, unsigned int resource, struct rlimit64 *rlim)
{
	return syscall(__NR_prlimit64, pid, resource, NULL, rlim);
}

int setprlimit(pid_t pid, unsigned int resource, const struct rlimit64 *rlim)
{
	return syscall(__NR_prlimit64, pid, resource, rlim, NULL);
}

#define MAKE_RES(x) { #x, x }

static const struct {
	const char *name;
	unsigned int res;
} limes[] = {
	MAKE_RES(RLIMIT_CPU),
	MAKE_RES(RLIMIT_FSIZE),
	MAKE_RES(RLIMIT_DATA),
	MAKE_RES(RLIMIT_STACK),
	MAKE_RES(RLIMIT_CORE),
	MAKE_RES(__RLIMIT_RSS),
	MAKE_RES(__RLIMIT_NPROC),
	MAKE_RES(RLIMIT_NOFILE),
	MAKE_RES(__RLIMIT_MEMLOCK),
	MAKE_RES(RLIMIT_AS),
	MAKE_RES(__RLIMIT_LOCKS),
	MAKE_RES(__RLIMIT_SIGPENDING),
	MAKE_RES(__RLIMIT_MSGQUEUE),
	MAKE_RES(__RLIMIT_NICE),
	MAKE_RES(__RLIMIT_RTPRIO),
	MAKE_RES(__RLIMIT_NLIMITS),
};

#undef MAKE_RES
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define LIMES_SIZE (ARRAY_SIZE(limes) - 1)

static void lim_write(pid_t pid, unsigned int resource, __u64 cur, __u64 max)
{
	struct rlimit64 r = { .rlim_cur = cur, .rlim_max = max };
	unsigned int a;

	for (a = 0; a < LIMES_SIZE; a++)
		if (limes[a].res == resource)
			break;
	printf("Writing %20s cur=%.25llu max=%.25llu\n", limes[a].name,
		cur, max);
	if (setprlimit(pid, resource, &r))
		err(1, "setprlimit");
}

static char *get_lim_text(__u64 lim, char *buf)
{
	if (lim == RLIM64_INFINITY)
		sprintf(buf, "%25s", "unlimited");
	else
		sprintf(buf, "%25llu", lim);

	return buf;
}

static void lim_read(pid_t pid)
{
	unsigned int a;

	for (a = 0; a < LIMES_SIZE; a++) {
		struct rlimit64 r;
		char cur[64], max[64];
		if (getprlimit(pid, limes[a].res, &r))
			err(1, "getprlimit");
		printf("%20s:%s%s\n", limes[a].name,
				get_lim_text(r.rlim_cur, cur),
				get_lim_text(r.rlim_max, max));
	}
}

int main(int argc, char **argv)
{
	pid_t pid;
	unsigned int wr = 0;

	if (argc < 3)
		goto usage;

	if (argv[2][0] == 'r')
		wr = 1;
	else if (argv[2][0] == 'w')
		wr = 2;

	if (!wr || argv[2][1] != 0)
		goto usage;
	wr--;

	pid = strtol(argv[1], NULL, 0);

	printf("%d\n", pid);
	if (wr) {
		__u64 cur = RLIM64_INFINITY, max = RLIM64_INFINITY;
		if (argc < 6)
			goto usage;
		if (strcmp(argv[4], "u"))
			cur = strtoull(argv[4], NULL, 0);
		if (strcmp(argv[5], "u"))
			max = strtoull(argv[5], NULL, 0);
		lim_write(pid, strtoul(argv[3], NULL, 0), cur, max);
		puts("rereading:");
		lim_read(pid);
	} else
		lim_read(pid);
	return 0;
usage:
	printf("usage: %s <pid> r|(w <limit number> <cur limit> <max limit>)\n", argv[0]);
	return 0;
}
