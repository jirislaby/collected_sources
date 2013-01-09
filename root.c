#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv, char **envp)
{
	setuid(0);
	execve("/bin/sh", argv, envp);

	return 1;
}
