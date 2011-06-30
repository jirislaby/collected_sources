#include <ctype.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CHARS_PER_LINE		32

static void print_hex_dump_bytes(const char *prefix_str, const void *vbuf,
		size_t len)
{
	const unsigned char *buf = vbuf;
	unsigned int a, b;
	char lbuf[CHARS_PER_LINE * 3 + 5 + CHARS_PER_LINE + 1];
	char *pos;

	if (strlen(prefix_str) > 10)
		return;

	for (a = 0; a < len / CHARS_PER_LINE; a++) {
		pos = lbuf;
		for (b = 0; b < CHARS_PER_LINE; b++) {
			pos += sprintf(pos, "%.2x ", buf[b]);
			if (!((b + 1) % 8) && b != CHARS_PER_LINE - 1)
				*pos++ = ' ';
		}
		for (b = 0; b < CHARS_PER_LINE; b++, buf++)
			*pos++ = isascii(*buf) && isprint(*buf) ? *buf : '.';
		*pos = '\0';

		printf("%s%s\n", prefix_str ? : "", lbuf);
	}
}

void __attribute__((constructor)) cons(void)
{
	FILE *file;
	char *name, *xname;
	unsigned long start, end;
	unsigned int inode;
	ssize_t rd;
	char line[256];

	rd = readlink("/proc/self/exe", line, sizeof(line) - 1);
	if (rd < 0)
		err(1, "readlink exe");
	line[rd] = 0;
	xname = strdup(line);
	if (!xname)
		errx(1, "strdup");

	file = fopen("/proc/self/maps", "r");
	if (file == NULL)
		err(1, "open maps");
	while (fgets(line, sizeof(line), file)) {
		int scanned = sscanf(line, "%lx-%lx %*4c %*x %*x:%*x %u %ms",
				&start, &end, &inode, &name);
		if (scanned == 3)
			continue;
		if (scanned != 4)
			err(1, "fscanf %d", scanned);
		if (*name != '/')
			goto next;
		if (strcmp(xname, name))
			goto next;
		printf("%lx-%lx: %s\n", start, end, name);
		print_hex_dump_bytes("", (void *)start, end - start);
next:
		free(name);
	}
	fclose(file);
}

int main(void)
{
//	cons();
	return 0;
}
