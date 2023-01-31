#include <err.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define min(a, b)	((a) < (b) ? (a) : (b))

static void dump_data(const char *prefix, const uint8_t *data, unsigned len)
{
	printf(" %s=", prefix);
	for (unsigned a = 0; a < len; a++)
		printf(" %.2x", data[a]);
}

static void dump_data_limited(const char *prefix, const uint8_t *data,
			      unsigned len, unsigned limit)
{
	dump_data(prefix, data, min(len, limit));
	if (len > limit)
		printf(" (%u more)", len - limit);
}


struct header {
	uint8_t FA;
	/*
	 * 4 bits: type
	 * 4 bits: seq_bot
	 * 6 bits: seq_top
	 * 2 bits: ???
	 * 4 bits: seq_top_top
	 * 1 bit: FINAL
	 * 1 bit: ???
	 * 1 bit: AUDIO
	 */
	uint8_t type_seq;
	uint8_t seq_res;
	uint8_t res_stream;
} __attribute__((packed));

#define PACKET_SIZE		184
#define HEADER_TY(x)		(((x)->type_seq & 0xf0) >> 4)
#define HEADER_TY1(x)		((x)->type_seq & 0x80)
#define HEADER_TY2(x)		((x)->type_seq & 0x40)
#define HEADER_TY3(x)		((x)->type_seq & 0x20)
#define HEADER_TY4(x)		((x)->type_seq & 0x10)
#define HEADER_TY_XXX(x)	((x)->type_seq & 0xf0)
#define  DATA_8			0x2
#define  DATA_148		0x5
#define  DATA_60		0xe
#define HEADER_res1(x)		((x)->seq_res & 2)
#define HEADER_res2(x)		((x)->seq_res & 1)
#define HEADER_REAL(x)		((x)->res_stream & 0x80)
#define HEADER_AUDIO(x)		((x)->res_stream & 0x40)
#define HEADER_SPEC(x)		((x)->res_stream & 0x20)
#define HEADER_FINAL(x)		((x)->res_stream & 0x10)
#define HEADER_XXX(x)		((x)->res_stream & 0xf0)
#define HEADER_SY(x)		((x)->res_stream & 0x0f)

static inline uint16_t HEADER_SEQ(const struct header *hdr)
{
	uint16_t seq = (hdr->seq_res & ~3) << 2;
	seq |= hdr->type_seq & 0x0f;

	return seq;
}

static unsigned get_data_size(const struct header *header, bool *old)
{
	*old = 0;
	if (HEADER_res2(header))
	    return 68;

	if (!HEADER_res1(header) && !HEADER_XXX(header)) {
		if (HEADER_SEQ(header) == 0x10)
			return 4;
		return 0;
	}

	if (!HEADER_res1(header) && HEADER_XXX(header) == 0xf0)
		return 12;

	switch (HEADER_TY(header)) {
	case DATA_8:
		*old = 1;
		return 8;
	case DATA_148:
		*old = 1;
		return 148;
	case DATA_60:
		*old = 1;
		return 56;
	default:
		return PACKET_SIZE - sizeof(*header);
	}
}

int main()
{
	struct header header;
	uint8_t buf[184 - sizeof(header)];
	unsigned int pkt = 0;
	ssize_t rd, to_read;
	size_t off = 0, skip = 0;

	while (1) {
		printf("off=%6zx", off);
		to_read = sizeof(header) - skip;
		rd = read(STDIN_FILENO, (void *)&header + skip, to_read);
		if (!rd)
			break;

		if (rd != to_read) {
			puts("");
			fflush(stdout);
			err(1, "read(header)");
		}

		skip = 0;
		off += rd;

		if (header.FA != 0xfa) {
			unsigned skipped = 0;
			do {
				rd = read(STDIN_FILENO, &header, 1);
				skipped++;
				off++;
			} while (rd > 0 && header.FA != 0xfa);
			if (rd <= 0)
				break;
			printf(" BAD; skipped: %u\n", skipped);
			skip = 1;
			continue;
		}

		dump_data("hdr", (const void *)&header, sizeof(header));

		printf(" SEQ=%4u/%3x%s SY=%u T=%u%u%u%u R=%u%u %c%c%c%c",
		       HEADER_SEQ(&header), HEADER_SEQ(&header),
		       (HEADER_SEQ(&header) & 1) ? " ODD" : "",
		       HEADER_SY(&header),
		       !!HEADER_TY1(&header), !!HEADER_TY2(&header),
		       !!HEADER_TY2(&header), !!HEADER_TY3(&header),
		       !!HEADER_res1(&header), !!HEADER_res2(&header),
		       HEADER_REAL(&header) ? 'R' : '_',
		       HEADER_AUDIO(&header) ? 'A' : '_',
		       HEADER_SPEC(&header) ? 'X' : '_',
		       HEADER_FINAL(&header) ? 'F' : '_');

		bool old;
		to_read = get_data_size(&header, &old);
		printf(" len=%3zd %c", to_read, old ? '!' : '_');
		if (to_read) {
			rd = read(STDIN_FILENO, buf, to_read);
			if (rd != to_read) {
				puts("");
				fflush(stdout);
				err(1, "read(data) at 0x%zx", off);
			}
//#define DUMP_RAW
			if (to_read == 180) {
				if (HEADER_SPEC(&header))
					printf(" S");
				else if (HEADER_AUDIO(&header)) {
					printf(" A");
#ifdef DUMP_AUDIO
					write(2, buf, rd);
#endif
				} else {
					printf(" V");
#ifdef DUMP_VIDEO
					write(2, buf, rd);
#endif
				}
			} else
				printf(" _");

			off += rd;
			dump_data_limited("payl", buf, rd, 20);
		}

		puts("");

		pkt++;
	}

	puts("");

	return 0;
}

