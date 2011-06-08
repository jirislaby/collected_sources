#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

void run(const char *program, const char* arg, char *const* env)
{
#ifdef DEBUG
    printf("program: %s, param: %s\n", program, arg);
#endif		
    execle(program, arg, env);
    perror("exec");
    _exit(10);
}

int main(int argc, char *const*argv, char *const* env)
{
    argc--; argv++;

    if (!argc)
    {
	fprintf(stderr, "Number of parameters must be at least 2!\n");
	return 1;
    }

    if (((argc >> 1) << 1) != argc)
    {
	fprintf(stderr, "Number of parameters is odd!\n");
	return 2;
    }

    for (; argc > 2; argc -= 2)
    {
	switch (vfork())
	{
	    case -1:
		perror("vfork error");
		break;
	    case 0:
		break;
	    default:
		run(argv[0], argv[1], env);
	}
	argv += 2;
    }
puts("xxx");
    run(argv[0], argv[1], env);

    return 10;
}
