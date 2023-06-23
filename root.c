#include <sys/types.h>
#include <unistd.h>

int main(int, char **argv, char **envp)
{
	setuid(0);
	execve("/bin/sh", argv, envp);

	return 1;
}
