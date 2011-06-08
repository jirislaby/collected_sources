#include <stdio.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>

int main()
{
puts("ble");
    struct sockaddr_un name;
    socklen_t len = sizeof(name);

    if (getsockname(0, (struct sockaddr*)&name, &len))
    {
	perror("gsn");
	return 1;
    }
    printf("%d, %s\n", name.sun_family, name.sun_path);
}
