#include <asm/types.h>
#define SWAP(a,b) asm("xchg %0,%1" : "+r" (a), "+r" (b) )

main()
{
register int a=1,b=2;
SWAP(a,b);
printf("%d, %d\n", a,b);
}
