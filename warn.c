#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

typedef void callback_t(void *);

void my_callback(void *arg)
{
	int fd = (int)arg;   /* 1 */
	close(fd);
}

int main() {

	int fd = open("aaa", O_RDONLY|O_CREAT, 0666);

	my_callback((void *)fd); /* 2 */
return 0;
}
