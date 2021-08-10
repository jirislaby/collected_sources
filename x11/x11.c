#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>

int main(int argc, char **argv)
{
	XInitThreads();

	Display *d = XOpenDisplay(NULL);
	printf("%p\n", d);
	XCloseDisplay(d);

	return 0;
}

