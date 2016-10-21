#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

static pthread_once_t once = PTHREAD_ONCE_INIT;

static void once_x()
{
	puts("ahoj");
}

static void *thr(void *d)
{
	unsigned a;
	printf("%2ld\n", (long)d); 
	for (a = 0; a < 1000; a++)
		pthread_once(&once, once_x);
	return NULL;
}

#define THREADS 50

int main()
{
	pthread_t t[50];
	unsigned i;

	for (i = 0; i < THREADS; i++)
		pthread_create(&t[i], NULL, thr, (void *)(long)i);

	for (i = 0; i < THREADS; i++)
		pthread_join(t[i], NULL);

	return 0;
}
