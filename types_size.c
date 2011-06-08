#include <stdio.h>
int main()
{
printf("%d %d=%d ", sizeof(long long), sizeof(long), sizeof(int));
printf("%d=%d %d\n", sizeof(short int), sizeof(short), sizeof(char));
printf("%d %d\n", sizeof(double), sizeof(float));
printf("%llu\n", ~0ull);
return 0;
}
