#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char **argv)
{
        char *ss;
        int shm = atoi(argv[1]);

        ss = shmat(shm, NULL, 0);
        if (ss == (void *)-1)
                err(2, "shmat");

	strcpy(ss, argv[2]);

	shmdt(ss);

	return 0;
}

