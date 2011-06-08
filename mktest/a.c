#include <stdio.h>
#include <config.h>

int main()
{
	puts("You have "
#if HAVE_LIBASOUND != 1
	"un"
#endif
	"usable alsa for compilation (devel packages).");
	return 0;
}
