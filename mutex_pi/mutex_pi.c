#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/time.h>
#include <sys/wait.h>

#define MAX_WAIT_USEC (1000*1000)
#define CHILDREN 16
#define MAX_ITER 200

#define NS_PER_S 1000000000

static pthread_mutex_t *proc_lock;

static void child()
{
	for (unsigned int i = 0; i < MAX_ITER; i++) {
		unsigned int wait_usec = 0;
		struct timespec abstime;
		int rv;

		while (1) {
			clock_gettime(CLOCK_REALTIME, &abstime);

			abstime.tv_nsec += 1000;
			if (abstime.tv_nsec >= NS_PER_S) {
				abstime.tv_sec++;
				abstime.tv_nsec -= NS_PER_S;
			}

			rv = pthread_mutex_timedlock(proc_lock, &abstime);
			if (rv == 0)
				break;
			if (rv == EOWNERDEAD) {
				pthread_mutex_consistent(proc_lock);
				break;
			}
			if (rv != ETIMEDOUT) {
				fprintf(stderr, "BADx rv=%d\n", rv);
				abort();
			}

			if (++wait_usec >= MAX_WAIT_USEC) {
				fprintf(stderr, "TIME OUT\n");
				abort();
			}
		}

		usleep(1);
		if (pthread_mutex_unlock(proc_lock))
			abort();
	}

	exit(0);
}

int main(void)
{
	proc_lock = mmap(NULL, sizeof(*proc_lock),
			 PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,
			 -1, 0);

	pthread_mutexattr_t mattr;

	pthread_mutexattr_init(&mattr);
	pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
	pthread_mutexattr_setrobust(&mattr, PTHREAD_MUTEX_ROBUST);
	pthread_mutexattr_setprotocol(&mattr, PTHREAD_PRIO_INHERIT);

	pthread_mutex_init(proc_lock, &mattr);

	pthread_mutexattr_destroy(&mattr);

	for (unsigned a = 0; a < CHILDREN; a++)
		if (!fork())
			child();

	for (unsigned a = 0; a < CHILDREN; a++) {
		int s;
		pid_t ch = wait(&s);
		if (WIFSIGNALED(s))
			fprintf(stderr, "child %d failed\n", ch);
	}

	pthread_mutex_destroy(proc_lock);

	return 0;
}
