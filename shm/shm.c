#include <err.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

int main(void)
{
	char *ss;
	int shm;

	shm = shmget(IPC_PRIVATE, 4096, S_IRUSR | S_IWUSR);
	if (shm < 0)
		err(1, "shmget");

	ss = shmat(shm, NULL, 0);
	if (ss == (void *)-1)
		err(2, "shmat");

	printf("ID: %d\n", shm);

	while (1) {
		puts(ss);
		sleep(1);
	}
}
