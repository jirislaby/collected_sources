#include <err.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include <sys/ioctl.h>

#define    BOTHER 0010000

struct query {
	union {
		struct {
			unsigned char res0:7,	// 0
				query:1;	// 1
			unsigned char res1;	// 0
			unsigned char a0;	// 0xa0
			unsigned char unit:4,
				 group:4;
			unsigned char on:1,
				 exe:1,
				 lock:1,
				 res2:1,	// 0
				 plasma:1,
				 res3:3;	// 0
			unsigned char mode:3,
				 auto_swing:1,
				 fan_speed:3,
				 res4:1;	// 0
			unsigned char temp:5,
				 res5:3;	// 0
		} __attribute__((packed));
		unsigned char payload[7];
	} u;
	unsigned char checksum;
} __attribute__((packed));

struct response {
	unsigned char crap1[7];
	union {
		struct {
			unsigned char len:7,
				      res0:1;
			unsigned char on:1,
				      res1:1,
				      lock:1,
				      res2:5;
			unsigned char res3[2];
			unsigned char unit:4,
				      group:4;
			unsigned char error;
			unsigned char mode:3,
				      auto_swing:1,
				      fan_speed:3,
				      res4:1;
			unsigned char temp:4,
				      res5:1,
				      plasma:1,
				      res6:2;
			unsigned char room_temp;
			unsigned char res7[6];
		} __attribute__((packed));
		unsigned char payload[15];
	} u;
	unsigned char checksum;
	unsigned char crap2[1];
} __attribute__((packed));

static int init_serial(const char *name)
{
	struct termios tio;
	int fd;
       
	fd = open(name, O_RDWR | O_NONBLOCK);
	if (fd < 0)
		err(1, "open");

	if (tcgetattr(fd, &tio))
                err(1, "cannot do tcgetattr");

	cfmakeraw(&tio);
	tio.c_cflag &= ~(CBAUD | CRTSCTS | CSTOPB);
	tio.c_cflag |= B4800 | CLOCAL | CREAD;

        if (tcsetattr(fd, TCSANOW, &tio))
                err(1, "cannot do tcsetattr");

	if (tcflush(fd, TCIOFLUSH))
                err(1, "cannot do tcflush");

	return fd;
}

static unsigned char compute_checksum(unsigned char *payload,
		unsigned int len)
{
	unsigned char sum = 0;
	int a;

	for (a = 0; a < len; a++)
		sum += payload[a];

	return sum ^ 0x55;
}

static const unsigned char room_temp_table[][2] = {
	{ 76, 77 }, /* 40 */
	{ 78, 79 }, /* 39 */
	{ 80, 82 }, /* 38 */
	{ 83, 84 }, /* 37 */
	{ 85, 86 }, /* 36 */
	{ 87, 89 }, /* 35 */
	{ 90, 92 }, /* 34 */
	{ 93, 94 }, /* 33 */
	{ 95, 97 }, /* 32 */
	{ 98, 99 }, /* 31 */
	{ 100, 102 }, /* 30 */
	{ 103, 105 }, /* 29 */
	{ 106, 108 }, /* 28 */
	{ 109, 111 }, /* 27 */
	{ 112, 113 }, /* 26 */
	{ 114, 116 }, /* 25 */
	{ 117, 119 }, /* 24 */
	{ 120, 122 }, /* 23 */
	{ 123, 125 }, /* 22 */
	{ 126, 128 }, /* 21 */
	{ 129, 131 }, /* 20 */
	{ 132, 134 }, /* 19 */
	{ 135, 137 }, /* 18 */
	{ 138, 141 }, /* 17 */
	{ 142, 144 }, /* 16 */
	{ 145, 147 }, /* 15 */
	{ 148, 150 }, /* 14 */
	{ 151, 153 }, /* 13 */
	{ 154, 156 }, /* 12 */
	{ 157, 159 }, /* 11 */
	{ 160, 162 }, /* 10 */
};

#define ARRAY_SIZE(x)	(sizeof(x) / sizeof(*(x)))

static unsigned char room_temp_to_C(unsigned char temp)
{
	int a;

	for (a = 0; a < ARRAY_SIZE(room_temp_table); a++)
		if (room_temp_table[a][0] <= temp && temp <= room_temp_table[a][1])
			return 40 - a;

	return 0;
}

static unsigned char temp_to_C(unsigned char temp)
{
	return temp + 15;
}

static __attribute__((unused)) unsigned char C_to_temp(unsigned char temp)
{
	int ret = (int)temp - 15;

	if (ret < 0x03)
		return 0x03;
	if (ret > 0x0f)
		return 0x0f;

	return ret;
}

enum Mode {
	Mode_cool = 0,
	Mode_dry = 1,
	Mode_fan = 2,
	Mode_auto = 3,
	Mode_heat = 4,
	Mode_first = Mode_cool,
	Mode_last = Mode_heat,
};

enum Fan_speed {
	Fan_lo = 1,
	Fan_mid = 2,
	Fan_hi = 3,
	Fan_auto = 4,
	Fan_verylo = 5,
	Fan_veryhi = 6,
	Fan_first = Fan_lo,
	Fan_last = Fan_veryhi,
};

int main(int argc, char *argv[])
{
	struct query query = {
		.u = {
			.query = 1,
			.a0 = 0xa0,
		},
	};
	struct response resp = {};
	unsigned char unit;
	int fd, a, opt;
	char opt_onoff = -1,
	    opt_fan = -1,
	    opt_mode = -1,
	    opt_plasma = -1,
	    opt_swing = -1,
	    opt_temp = -1;
	bool set = false;
	ssize_t rd;
	char *eptr;

	while ((opt = getopt(argc, argv, "01f:m:p:s:t:")) >= 0) {
		switch (opt) {
		case '0':
			opt_onoff = 0;
			set = true;
			break;
		case '1':
			opt_onoff = 1;
			set = true;
			break;
		case 'f':
			opt_fan = atoi(optarg);
			if (opt_fan < Fan_first || opt_fan > Fan_last)
				opt_fan = -1;
			else
				set = true;
			break;
		case 'm':
			opt_mode = atoi(optarg);
			if (opt_mode < Mode_first || opt_mode > Mode_last)
				opt_mode = -1;
			else
				set = true;
			break;
		case 'p':
			opt_plasma = !!atoi(optarg);
			set = true;
			break;
		case 's':
			opt_swing = !!atoi(optarg);
			set = true;
			break;
		case 't':
			opt_temp = C_to_temp(atoi(optarg));
			set = true;
			break;
		default:
			errx(1, "bad use");
			return 0;
		}
	}

	if ((argc - optind) < 2)
		errx(2, "bad use");

	fd = init_serial(argv[optind]);

	unit = strtoul(argv[optind + 1], &eptr, 16);
	if (*eptr != 0)
		errx(1, "bad unit number");

	query.u.group = unit >> 4;
	query.u.unit = unit;

	query.checksum = compute_checksum(query.u.payload,
			sizeof(query.u.payload));

	for (a = 0; a < 10; a++) {
		if (query.u.exe)
			puts("EXE");
		write(fd, &query, sizeof(query));
		usleep(200000);
		rd = read(fd, &resp, sizeof(resp));
		if (rd > 0) {
			int a;
			printf("len=%.2zd", rd);
			for (a = 0; a < rd; a++)
				printf(" %.2x", ((unsigned char *)&resp)[a]);
			puts("");
		}

		if (rd < sizeof(sizeof(resp)))
			break;

		if (compute_checksum(resp.u.payload, sizeof(resp.u.payload)) != resp.checksum) {
			printf("    BAD checksum\n");
			continue;
		}

		printf("    len=%.2d on=%d lck=%d unit=%.2x err=%.2x mode=%d sw=%d fan=%d temp=%dC pls=%d rtemp=%d(%dC)\n",
				resp.u.len,
				resp.u.on,
				resp.u.lock,
				resp.u.group << 4 | resp.u.unit,
				resp.u.error,
				resp.u.mode,
				resp.u.auto_swing,
				resp.u.fan_speed,
				temp_to_C(resp.u.temp),
				resp.u.plasma,
				resp.u.room_temp,
				room_temp_to_C(resp.u.room_temp));

		if (set) {
			query.u.fan_speed =	opt_fan >= 0	? opt_fan	: resp.u.fan_speed;
			query.u.mode =		opt_mode >= 0	? opt_mode	: resp.u.mode;
			query.u.on =		opt_onoff >= 0	? opt_onoff	: resp.u.on;
			query.u.plasma =	opt_plasma >= 0	? opt_plasma	: resp.u.plasma;
			query.u.auto_swing =	opt_swing >= 0	? opt_swing	: resp.u.auto_swing;
			query.u.temp =		opt_temp >= 0	? opt_temp	: resp.u.temp;
			query.u.exe = 1;
			query.checksum = compute_checksum(query.u.payload,
					sizeof(query.u.payload));

			set = false;
		} else if (query.u.exe) {
			query.u.exe = 0;
			query.checksum = compute_checksum(query.u.payload,
					sizeof(query.u.payload));
		}

		usleep(200000);
	}

	close(fd);

	return 0;
}
