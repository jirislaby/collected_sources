#include <err.h>
#include <stdio.h>

#include "ascii.h"

int main(void)
{
	unsigned int w, h, b, p = p;
	char buf[100];

	(void)width; (void)height; (void)header_data_cmap;

	puts("static uint8_t charset[][] = {");
	for (h = 0; h < height; h++) {
		for (b = 0, w = 26; w <= 29; w++, b <<= 1)
			b |= !!header_data[h * width + w];

		b >>= 1;

		if (!(h % 7))
			p = sprintf(buf, "\t{ ");
		p += sprintf(buf + p, "0x%x, ", b);
		if (!((h+1) % 7)) {
			sprintf(buf + p - 2, " }, /* %c */", h/7 + 169);
			p = 0;
			puts(buf);
		}
	}
	puts("};");

	return 0;
}
