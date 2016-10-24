#include <err.h>
#include <limits.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main()
{
	long msg[] = { 100, 200 };
	int m = msgget(IPC_PRIVATE, IPC_CREAT | 0644);
	msgsnd(m, &msg, sizeof(msg), 0);
	msgrcv(m, &msg, sizeof(msg), LONG_MIN, 0);
}
