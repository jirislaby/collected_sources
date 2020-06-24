#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <linux/vt.h>

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

static void test_reads(int tty, int vcs, struct coord *winsz,
		struct coord *cursor)
{
	struct header {
		unsigned char rows, cols, x, y;
	} header;
	//char buf[16];
	ssize_t rd;
	unsigned int off;

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

	cursor.x = 10;
	cursor.y = 20;

	goto_xy(tty, &cursor);
	write(tty, "aho\r\n", 5);

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

