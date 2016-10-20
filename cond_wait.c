#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

static pthread_mutex_t m;
static pthread_cond_t cond;
static pthread_cond_t condw;
static char ccc;

static void *thr(void *d)
{
	pthread_mutex_lock(&m);
	while (1) {
		int ret = pthread_cond_wait(&cond, &m);
		printf("read %c ret=%d\n", ccc, ret);
		pthread_cond_broadcast(&condw);
		usleep(100000);
	}
	pthread_mutex_unlock(&m);
	return NULL;
}

int main()
{
	pthread_t t;
	int c;

	pthread_mutex_init(&m, NULL);
	pthread_cond_init(&cond, NULL);
	pthread_cond_init(&condw, NULL);

	pthread_create(&t, NULL, thr, NULL);

	while ((c = getchar()) >= 0) {
		if (!isprint(c))
			continue;
		printf("writing %c\n", c);
		pthread_mutex_lock(&m);
		ccc = c;
		pthread_cond_broadcast(&cond);
		pthread_cond_wait(&condw, &m);
		pthread_mutex_unlock(&m);
	}

	pthread_cancel(t);
	pthread_join(t, NULL);

	return 0;
}
