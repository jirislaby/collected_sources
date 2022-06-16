#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <getopt.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	if (argc < 2) {
		char *fl = getenv("MAKEFLAGS");
		if (!fl || !strlen(fl))
			errx(1, "no MAKEFLAGS");

		char res[128];
		strcpy(res, argv[0]);
		strcat(res, " ");
		strcat(res, fl);
		printf("reexecuting with: %s\n", res);
		fflush(stdout);

		execl("/bin/sh", "sh", "-c", res, NULL);
	}

	static const struct option longopts[] = {
		{ "jobserver-auth", 1, NULL, 'j' },
		{}
	};
	char *fds = NULL;
	int val;

	opterr = 0;
	while ((val = getopt_long(argc, argv, "", longopts, NULL)) >= 0)
		switch (val) {
		case 'j':
			fds = optarg;
			break;
		}

	if (!fds)
		errx(1, "no jobserver-auth");

	printf("fds=%s\n", fds);
	int fd1 = atoi(strtok(fds, ","));
	int fd2 = atoi(strtok(NULL, ","));
	printf("fds parsed: %d and %d\n", fd1, fd2);

	char buf[8];
	unsigned count;
	for (count = 0; count < sizeof(buf); count++) {
		ssize_t rd1 = read(fd1, &buf[count], 1);
		if (rd1 < 0)
			break;
		printf("ch%d=%c\n", count, buf[count]);
	}

	while (count--)
		write(fd2, &buf[count], 1);

	return 0;
}

