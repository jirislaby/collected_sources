#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <linux/vt.h>

#define POISON		128
#define POISON_FUN(x)	(0xe0 | ((x) & 0xf))
#define START_LETTER	' '

struct coord {
	unsigned short x, y;
};

static void goto_xy(int tty, struct coord *cur)
{
	char buf[16];
	int len = sprintf(buf, "\e[%u;%uH", cur->y, cur->x);

	write(tty, buf, len);
}

static void get_cursor(int tty, struct coord *cur)
{
	char buf[16];
	ssize_t rd;

	write(tty, "\e[6n", 4);
	rd = read(tty, buf, sizeof(buf) - 1);
	if (rd < 0)
		err(1, "read cursor pos");

	buf[rd] = 0;

	if (sscanf(buf, "\e[%hu;%huR", &cur->y, &cur->x) != 2)
		errx(1, "cannot parse cur pos");
}

static int tty_setup(const char *dev, struct coord *winsz,
		struct coord *cur)
{
	struct winsize _winsz;
	struct termios tio;
	int tty;

	tty = open(dev, O_RDWR);
	if (tty < 0)
		err(1, "open %s", dev);

	if (ioctl(tty, TIOCGWINSZ, &_winsz) < 0)
		err(1, "ioctl(TIOCGWINSZ)");

	winsz->x = _winsz.ws_col;
	winsz->y = _winsz.ws_row;

	if (tcgetattr(tty, &tio) < 0)
		err(1, "tcgetattr");
	cfmakeraw(&tio);
	if (tcsetattr(tty, TCSANOW, &tio) < 0)
		err(1, "tcgetattr");

	get_cursor(tty, cur);

	return tty;
}

static void hex_dump(const void *_buf, unsigned int len, unsigned short cols)
{
	const char *buf = _buf;
	unsigned int i;

	for (i = 0; i < len; i++) {
		unsigned char c = buf[i];
		printf("\e[100m");
		putchar(isprint(c) && !isspace(c) ? c : ' ');
		printf("\e[0m");
		printf("%.2x", c);
		if (!((i + 1) % cols))
			puts("");
	}

	puts("");
}

static void check_poison(const char *info, const char *poison)
{
	unsigned int i;

	for (i = 0; i < 4; i++) {
		unsigned char data = poison[i];
		unsigned char exp = POISON_FUN(i);

		if (data != exp) {
			printf("bad poison (%s): %.2x != %.2x at %u\n", info, data, exp, i);
			hex_dump(poison, POISON, 16);
			break;
		}
	}
}

static void test_reads(int tty, int vcs, struct coord *winsz,
		struct coord *cursor)
{
	struct attr {
		unsigned char rows, cols, x, y;
		char data[];
	} header, *buf;
	char *alloc;
	ssize_t rd;
	unsigned int off, data_size, i;

	for (off = 0; off < 4; off++) {
		if (lseek(vcs, off, SEEK_SET) < 0)
			err(1, "seek vcs");

		rd = read(vcs, (void *)&header + off, sizeof(header) - off);
		if (rd < 0)
			err(1, "read vcs");

		printf("header: %ux%u [%u, %u]\n", header.cols, header.rows,
				header.x, header.y);

		if (header.cols != winsz->x || header.rows != winsz->y)
			errx(1 , "winsz doesn't match");

		if (header.x + 1 != cursor->x || header.y + 1 != cursor->y)
			errx(1 , "cursor doesn't match");
	}

	data_size = header.cols * header.rows * 2;
	alloc = malloc(sizeof(buf) + data_size + 2 * POISON);

	for (i = 0; i < POISON; i++)
		alloc[i] = POISON_FUN(i);
	buf = (void *)alloc + POISON;
	for (i = 0; i < POISON; i++)
		buf->data[data_size + i] = POISON_FUN(i);


	if (lseek(vcs, 0, SEEK_SET) < 0)
		err(1, "seek vcs");

#ifdef PRINT_SCREEN
	rd = read(vcs, buf, sizeof(buf) + data_size);
	if (rd < 0)
		err(1, "read vcs");

	for (i = 0; i < data_size / 2; i++) {
		printf("%c", buf->data[i * 2]);
		if (!((i + 1) % header.cols))
			puts("");
	}
	puts("");
#endif

	for (off = 0; off < 4; off++) {
		bool invalid = false;

		if (lseek(vcs, off, SEEK_SET) < 0)
			err(1, "seek vcs");

		memset(buf, 0, off);
		rd = read(vcs, buf + off, sizeof(buf) + data_size - off);
		if (rd < 0)
			err(1, "read vcs");

		for (i = 0; i < data_size / 2; i++) {
			unsigned int col = i % winsz->x;
			unsigned int row = i / winsz->x;
			char data = buf->data[i * 2];

			if (col == 0 && data != START_LETTER + row) {
				printf("invalid data at %5u [%3u, %3u] (off=%u): ", i, col, row, off);
				putchar(isprint(data) ? data : ' ');
				putchar(' ');
				printf(" %.2x (expected %c)\n", (unsigned char)data, START_LETTER + row);
				invalid = true;
			}
		}
		if (invalid) {
			hex_dump(buf, sizeof(buf) + data_size, header.cols / 2);
			break;
		}
	}

	check_poison("start", alloc);
	check_poison("end", &buf->data[data_size]);

	free(alloc);
}

static void fill_screen(int tty, struct coord *winsz)
{
	struct coord cursor = { 1, 1 };
	unsigned short x;

	write(tty, "\e[2J", 4);

	for (; cursor.y <= winsz->y; cursor.y++) {
		unsigned short c = START_LETTER + cursor.y - 1;

		goto_xy(tty, &cursor);
		for (x = 0; x < 48; x++, c++)
			write(tty, &c, 1);
	}
}

int main(int argc, char **argv)
{
	struct coord winsz, cursor;
	char dev[PATH_MAX];
	unsigned int con = 20;
	int vcs, tty;

	if (argc > 1)
		con = atoi(argv[1]);

	sprintf(dev, "/dev/tty%d", con);
	tty = tty_setup(dev, &winsz, &cursor);

	printf("Window: %ux%u\n", winsz.x, winsz.y);
	printf("Cursor: [%u, %u]\n", cursor.x, cursor.y);

	fill_screen(tty, &winsz);

	get_cursor(tty, &cursor);
	printf("Cursor: [%u, %u]\n", cursor.x, cursor.y);

	sprintf(dev, "/dev/vcsa%d", con);
	vcs = open(dev, O_RDWR);
	if (vcs < 0)
		err(1, "open %s", dev);

	test_reads(tty, vcs, &winsz, &cursor);

	close(vcs);

	/*if (ioctl(tty, VT_DISALLOCATE, con) < 0)
		err(1, "ioctl(VT_DISALLOCATE)");*/

	close(tty);

	return 0;
}

