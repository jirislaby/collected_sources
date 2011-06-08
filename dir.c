#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

int main(int argc, char**argv)
{
    struct dirent *de;
    DIR *dir = opendir(argv[1]);

    while ((de = readdir(dir)))
    {
	printf("%s\n", de->d_name);
    }

    closedir(dir);

    return 0;
}
