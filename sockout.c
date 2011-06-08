#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>

int main()
{
    struct sockaddr_un sa;
    sa.sun_family = 0;
    strcpy(sa.sun_path, "ble");
    close(0);
    dup2(socket(AF_UNIX, SOCK_STREAM, 0), 0);
    fcntl(0, F_SETFD, 0);
    if (bind(0, (struct sockaddr*)&sa, sizeof(sa)))
    {
	perror("bind");
	return 1;
    }

    execl("ac", "ac");
}
