#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>

int main(int argc, char **argv)
{
    struct servent *aaa = getservbyname(argv[1], "tcp");
    if (!aaa)
    {
	puts("Not found");
	return 1;
    }

    printf("%s, %d, alias: ", aaa->s_name, (aaa->s_port));

    while (aaa->s_aliases[0])
    {
	printf("%s ", aaa->s_aliases[0]);
	aaa->s_aliases++;
    }

    puts("");
}
