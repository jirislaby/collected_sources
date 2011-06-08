#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define RET(x) { close(sock); return (x); }

int main(int argc, char **argv)
{
	argc--, argv++;
	if (argc <= 1) return 1;

	struct protoent *pe;
	struct servent *se;

	if (!(pe = getprotobyname(argv[0])) &&
		!(pe = getprotobynumber(atoi(argv[0]))))
		return 2;

	argc--, argv++;

	if (!(se = getservbyname(argv[0], pe->p_name)) &&
		!(se = getservbyport(htons(atoi(argv[0])), pe->p_name)))
		return 3;

	printf("Using %s protocol, port %d (%s).\n", pe->p_name, ntohs(se->s_port),
			se->s_name);

	int sock;

	if ((sock = socket(AF_INET, SOCK_STREAM, pe->p_proto)) == -1)
		return 4;

	int a = 1;

	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &a, sizeof(a)) == -1)
		RET(5);

	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	sa.sin_port = se->s_port;

	if (bind(sock, (struct sockaddr*)&sa, sizeof(sa)) == -1)
		RET(6);

	if (listen(sock, 10))
		RET(7);

	socklen_t b = sizeof(sa);
	int conn;
	while ((conn = accept(sock, (struct sockaddr*)&sa, &b)) != -1)
	{
		printf("Addr: %s, port: %d\n", inet_ntoa(sa.sin_addr),
			   ntohs(sa.sin_port));
		char buf[128];
		int kill = 0;
		while (int len = read(conn, buf, 127))
		{
			buf[len] = 0;
			if (!strcmp(buf, "quit\r\n"))
				break;
			if (!strcmp(buf, "kill\r\n"))
			{
				kill = 1;
				break;
			}
			printf(buf);
			write(conn, buf, len);
		}
		close(conn);
		puts("Connection closed");
		if (kill)
			break;
	}
			
	close(sock);

	return 0;
}
