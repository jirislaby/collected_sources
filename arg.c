#include <stdio.h>
#include <alloca.h>

int data;

int funkce(int aaa)
{
    int bbb;
    void *ccc = alloca(1);

    printf("a: %p\nb: %p\nc: %p\nd: %p\nf: %p\n", (void*)&aaa, (void*)&bbb, ccc,
	    (void*)&data, &funkce);

    return 0;
}

int main()
{
    funkce(5);

    return 0;
}
