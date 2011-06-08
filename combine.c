#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *const*argv)
{
    int counter, fd;
    register char *ptr;

    argc--, argv++;

    char **ptrs = (char**)malloc(argc * sizeof(char*));

    for (counter = 0; counter < argc; counter++, argv++)
	if ((fd = open(argv[0], O_RDONLY)) == -1)
	    perror(argv[0]);
	else
	    ptrs[counter] = mmap(NULL, lseek(fd, SEEK_END, 0), PROT_READ,
		    MAP_SHARED, fd, 0);

    while (1)
    {
	for (counter = 0; counter < argc; counter++)
	    if (ptrs[counter] != -1)
	    {
		ptr = ptrs[counter];
		do
		{
		    
		} while ();
	    }
    }

    free(fds);

    return 0;
}
