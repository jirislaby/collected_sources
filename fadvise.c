#include <fcntl.h>

int main()
{
	posix_fadvise(0, 9223372036854771711, 7500402, POSIX_FADV_NORMAL);
}
