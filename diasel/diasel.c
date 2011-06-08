/*
 * Copyright (C) 2007 Jiri Slaby <jirislaby@gmail.com>
 *
 * This file is distributed under GPL2
 */

#include "diasel.h"

#define THRESH		160 /* consider ascii with codes >= this value */

void diasel(char *out, const char *in, unsigned char cnt)
{
	const unsigned char *tmp;
	unsigned char stat[256 - THRESH] = { 0, }, outa[cnt];
	unsigned char a, b, m, mpos;

	for (tmp = (unsigned char *)in; *tmp; tmp++)
		if (*tmp >= THRESH) {
			stat[*tmp - THRESH]++;
			/* disallow overflows */
			if (stat[*tmp - THRESH] == 0xff)
				break;
		}

	for (a = 0; a < cnt; a++)
		out[a] = outa[a] = 0;
	for (a = 0; a < sizeof(stat); a++)
		if (stat[a]) {
			/* find min */
			for (b = mpos = 0, m = 0xff; b < cnt; b++)
				if (m > outa[b]) {
					m = outa[b];
					mpos = b;
				}
			/* fill it in instead of the min */
			if (stat[a] > outa[mpos]) {
				outa[mpos] = stat[a];
				out[mpos] = a + THRESH;
			}
		}
}
