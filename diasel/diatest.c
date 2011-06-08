/*
 *  Copyright (C) 2007 Jiri Slaby <jirislaby@gmail.com>
 *
 *  This file is distributed under GPL2
 */

#include <err.h>
#include <stdio.h>
#include <unistd.h>

#include "diasel.h"

#define FREE_SPACE	8 /* how many chars we can define in the display */

int main(void)
{
	ssize_t cnt;
	unsigned int a, pos;
	char in[512], out[FREE_SPACE];

	cnt = read(STDIN_FILENO, in, sizeof(in));
	if (cnt < 0)
		err(1, "read error");
	if (!cnt)
		errx(0, "no input");

	diasel(out, in, sizeof(out));

	pos = sprintf(in, "These are the selected: [");
	for (a = 0; a < sizeof(out); a++)
		if (out[a])
			pos += sprintf(in + pos, "'%c', ", out[a]);
	pos -= 2;
	sprintf(in + pos, "]");
	puts(in);

	return 0;
}
