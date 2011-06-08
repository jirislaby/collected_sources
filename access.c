#include <unistd.h>

int main(int a, char **argv)
{
    return access(argv[1], R_OK);
}
