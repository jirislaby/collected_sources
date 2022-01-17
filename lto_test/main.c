#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"

int main(int argc, char **argv)
{
	unsigned int a;

	if (lib_prep() < 0)
		err(1, "lib_prep");

	for (a = 0; a < 100000; a++)
		lib_fun(a);

	lib_exit();

	return 0;
}

