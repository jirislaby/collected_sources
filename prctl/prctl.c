#include <sys/prctl.h>
#include <stdlib.h>

int main(void)
{
	prctl(PR_SET_NAME, "myprocess/1");
	abort();
	return 0;
}
