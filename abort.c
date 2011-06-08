#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int xxx;

    sync();

    xxx = 3;

    abort();

    return 0;
}
