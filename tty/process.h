#ifndef PROCESS_H
#define PROCESS_H

#include <err.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/wait.h>

typedef void (*callback)(void *arg);

struct th_hooks {
	callback post_fork;
	callback threads_created;
	callback threads_done;
	callback thread;
};

struct th_arg {
	const struct th_hooks *hooks;
	void *arg;
	unsigned int thread;
	unsigned int loops;
};

static inline void *__one_thread(void *arg)
{
	struct th_arg *th_arg = arg;
	unsigned int loop;
	callback cb = th_arg->hooks->thread;
	pid_t pid = getpid();

	printf("CHILD %5u-%.3u: calling %p %16u times\n", pid,
			th_arg->thread, cb, th_arg->loops);

	for (loop = 0; loop < th_arg->loops; loop++) {
		if (!(loop % 100000))
			printf("CHILD %5u-%.3u: loop %8u %p\n", pid,
					th_arg->thread, loop, cb);
		cb(th_arg->arg);
	}

	return NULL;
}

static inline void run_parallel_threads(unsigned short threads,
		unsigned int loops, const struct th_hooks *hooks, void *arg)
{
	struct th_arg th_arg[threads];
	pthread_t th[threads];
	unsigned short thread;

	for (thread = 0; thread < threads; thread++) {
		th_arg[thread].hooks = hooks;
		th_arg[thread].arg = arg;
		th_arg[thread].thread = thread;
		th_arg[thread].loops = loops;
		if (pthread_create(&th[thread], NULL, __one_thread,
					&th_arg[thread]))
			errx(1, "pthread_create");
	}

	if (hooks->threads_created)
		hooks->threads_created(arg);

	for (thread = 0; thread < threads; thread++)
		pthread_join(th[thread], NULL);

	if (hooks->threads_done)
		hooks->threads_done(arg);
}

static inline unsigned short run_parallel(unsigned short procs,
		unsigned short threads,
		unsigned int loops, const struct th_hooks *hooks, void *arg)
{
	unsigned short proc;
	pid_t pid;

	for (proc = 0; proc < procs; proc++) {
		pid = fork();
		switch (pid) {
		case -1:
			err(1, "fork");
		case 0:
			break;
		default:
			printf("PARENT: created %u\n", pid);
			continue;
		}
		if (hooks->post_fork)
			hooks->post_fork(arg);
		run_parallel_threads(threads, loops, hooks, arg);
		exit(0);
	}

	return procs;
}

static inline void wait_for_parallel(unsigned short procs)
{
	for (; procs; procs--) {
		pid_t pid = wait(NULL);
		printf("PARENT: reaped %u\n", pid);
	}
}

#endif
