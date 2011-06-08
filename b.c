#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

pid_t pid;

void toKill(int sigNO)
{
   exit(1); 
}

int main()
{
    int a=0;

    signal(SIGALRM,toKill); 

    alarm(1);

    while (1) //for (; a<10; a++)
      fork();

    return 0;
}
