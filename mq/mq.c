#include <err.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <limits.h>
#include <stdio.h>

int main(void)
{
	char qname[50];
	int a;
	printf("%u %u\n", _POSIX_OPEN_MAX, _POSIX_MQ_OPEN_MAX);
	for (a = 0; a < 10000; a++) {
		sprintf(qname, "/bubak%d", a);
		mqd_t ret = mq_open(qname, O_RDWR|O_CREAT, S_IRUSR | S_IWUSR, NULL);
		if (ret == (mqd_t)-1)
			err(1, "mq_open");
	}

	return 0;
}
