#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define odkud 'A'
#define pokud 'Z'
/*#define odkud 'a'
#define pokud ('z' + 10)*/

void funkce(const int c, const char pis[20])
{
	unsigned char x;

	if (c == 0)
		printf("%s\n",pis);  //vypise to jednu kombinacu
	else
		for (x = odkud; x <= pokud; x++) {
			char pom[20];
			register int a;

			strcpy(pom, pis);
			a = strlen(pom);
//			pom[a] = x <= 'z' ? x : x - 'z' + '0' - 1;
			pom[a] = x;
			pom[a+1] = 0;
			funkce(c-1, pom);
		}
}

int main(int argc, char **argv)
{
	int b;

	argc--, argv++;

	if (!argc)
		return 1;

	b = atoi(*argv);
	funkce(b, "");

	return 0;
}
