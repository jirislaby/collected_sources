#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>


void __attribute__((noreturn)) child(int in, int out)
{
	close(0); /* we don't need stdin, we have ours */
	close(1); /* dtto with stdout */

	if (dup2(in, 0) != 0 || /* we need fds[0] as stdin = 0 */
			dup2(out, 1) != 1) { /* stdout = 1 */
		perror("dup2");
		goto end;
	}

	if (fcntl(0, F_SETFD, 0) || /* don't close fd, when doing exec */
			fcntl(1, F_SETFD, 0)) {
		perror("fcntl");
		goto end;
	}

	execlp("sort", "sort", NULL);

	perror("exec"); /* we will reach this if exec fails */
end:
	exit(100);
}

int main(int argc, char **argv)
{
	int ret = 1, cnt = 0;
	int fds[2]; /* pipe, where I'll write to and child'll read from*/
	int fds1[2]; /* I'll read, child'll write here */

	if (pipe(fds)) {
		perror("pipe");
		goto end;
	}

	if (pipe(fds1)) {
		perror("pipe");
		ret++;
		goto errpip;
	}
	
again:
	switch (fork()) {
	case 0: /* fork OK, we can continue */
		break;
	case -1: /* fork NOK */
		if (errno == EAGAIN) {
			usleep(1e4); /* try to wait 10 ms and try fork again */
			if (cnt++ < 1000)
				goto again;
		}
		perror("fork");
		ret++;
		goto errfork;
	default: /* fork OK, we are child */
		close(fds[1]); /* we won't write to fds */
		close(fds1[0]); /* we won't read from fds1 */
		child(fds[0], fds1[1]);
	}

	close(fds[0]);
	close(fds1[1]);
	
	if (write(fds[1], "a\nc\nb\nd\n", 8) != 8) {
		perror("write");
		goto errwr;
	}
	close(fds[1]);

	puts("sort wrote:");

	char buf[16];
	while ((cnt = read(fds1[0], buf, sizeof(buf)))) {
		if (cnt < 0) {
			perror("read");
			goto errrd;
		}
		if (write(0, buf, cnt) != cnt) {
			perror("write to stdout");
			goto errrd;
		}
	}
	close(fds[0]);
	
	int st;
	wait(&st);

	return 0;
errwr:
	close(fds[1]);
errrd:
	close(fds[0]);

	return 50;
errfork:
	close(fds1[0]);
	close(fds1[1]);
errpip:
	close(fds[0]);
	close(fds[1]);
end:
	return ret;
}
