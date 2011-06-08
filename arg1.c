#include <stdio.h>

int main(int argc, char **argv)
{
    argc--; argv++;

    for(;argc > 2; argc-=2)
    {
	printf("%d: %s %s\n", argc, argv[0], argv[1]);
	argv+=2;
    }

    printf("%d: %s %s\n", argc, argv[0], argv[1]);
    return 0;
}
