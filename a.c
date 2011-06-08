#include <stdio.h>
#include <stdlib.h>

int main()
{
    int *ptr,*ptr1=(int*)malloc(1024*sizeof(int));
    int a;

    ptr1[0]=0;
    ptr1[1]=0xf0f0f0f0;
    ptr1[1023]=10;

    for (a=0;a<200000;a++) 
    	if ((ptr=(int*)malloc(1024*sizeof(int)))) ptr[0]=(int)ptr1, ptr[1023]=10, ptr1=ptr;

    puts("written");

    while (ptr[0])
    {
	if (ptr[1023]!=10) puts("ee");
#ifdef DEBUG
	printf("%p\n",ptr);
#endif
	ptr1=ptr;
	ptr=(int*)ptr[0];
	free(ptr1);
    }
    printf("%x\n",ptr[1]);
    free(ptr);
}
