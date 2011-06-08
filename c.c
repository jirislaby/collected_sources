#include <unistd.h>

int main()
{
  int a;
  fork();
  while (1)
  {
    a++;
    a/a;
  }
  
  return 0;
}
