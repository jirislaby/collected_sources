#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

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
	union {
		struct {
			uint8_t FA;
			/*
			 * 4 bits: type
			 * 4 bits: seq_bot
			 * 5 bits: seq_top
			 * 3 bits: ???
			 * 1 bit: FINAL
			 * 1 bit: SPEC
			 * 1 bit: AUDIO
			 * 1 bit: REAL
			 * 4 bits: synch
			 */
			uint8_t type_seq;
			uint8_t seq_res;
			uint8_t res_stream;
		};
		uint32_t val;
	};
} __attribute__((packed));

#define PACKET_SIZE		184
#define HEADER_FA(hdr)		(((hdr)->val & 0xff000000) >> 24)
#define HEADER_TY1(hdr)		(((hdr)->val & 0x00800000) >> 23)
#define HEADER_TY2(hdr)		(((hdr)->val & 0x00400000) >> 22)
#define HEADER_TY3(hdr)		(((hdr)->val & 0x00200000) >> 21)
#define HEADER_TY4(hdr)		(((hdr)->val & 0x00100000) >> 20)
#define HEADER_TY_MASK(hdr)	(((hdr)->val & 0x00f00000) >> 20)
#define  DATA_8				    0x2
#define  DATA_148			    0x5
#define  DATA_60			    0xe
#define HEADER_SEQ_BOT(hdr)	(((hdr)->val & 0x000f0000) >> 16)
#define  HEADER_SEQ_BOT_BITS	4
#define HEADER_SEQ_TOP(hdr)	(((hdr)->val & 0x0000f800) >> 11)
#define HEADER_res1(hdr)	(((hdr)->val & 0x00000400) >> 10)
#define HEADER_res2(hdr)	(((hdr)->val & 0x00000200) >>  9)
#define HEADER_res3(hdr)	(((hdr)->val & 0x00000100) >>  8)
#define HEADER_REAL(hdr)	(((hdr)->val & 0x00000080) >>  7)
#define HEADER_AUDIO(hdr)	(((hdr)->val & 0x00000040) >>  6)
#define HEADER_SYNC(hdr)	(((hdr)->val & 0x00000020) >>  5)
#define HEADER_FINAL(hdr)	(((hdr)->val & 0x00000010) >>  4)
#define HEADER_XXX_MASK(hdr)	(((hdr)->val & 0x000000f0) >>  4)
#define HEADER_SY(hdr)		(((hdr)->val & 0x0000000f) >>  0)

static inline uint16_t HEADER_SEQ(const struct header *hdr)
{
	uint16_t seq = HEADER_SEQ_TOP(hdr);

	seq <<= HEADER_SEQ_BOT_BITS;
	seq |= HEADER_SEQ_BOT(hdr);

	return seq;
}

static unsigned get_data_size(const struct header *header, bool *old)
{
	*old = 0;
	if (HEADER_res3(header))
	    return 68;

	if (!HEADER_res1(header) && !HEADER_XXX_MASK(header)) {
		if (HEADER_SEQ(header) == 0x10)
			return 4;
		return 0;
	}

	if (!HEADER_res2(header) && HEADER_XXX_MASK(header) == 0xf)
		return 12;

	switch (HEADER_TY_MASK(header)) {
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

#define PAD //"."

int main(int argc, char **argv)
{
	struct header header;
	uint8_t buf[184 - sizeof(header)], videobuf[720*576/2*2];
	unsigned int pkt = 0, pkt_lim = UINT_MAX;
	ssize_t rd, to_read;
	size_t off = 0, skip = 0;
	unsigned vsize = 0;
	bool synced = false;
	int o, video_fd = -1, audio_fd = -1, in_fd = STDIN_FILENO;

	memset(videobuf, 0, sizeof(videobuf));

	while ((o = getopt(argc, argv, "ai:l:v")) != -1) {
		switch (o) {
		case 'a':
			audio_fd = open("audio.raw", O_WRONLY | O_CREAT |
					O_TRUNC, 0644);
			break;
		case 'v':
			video_fd = open("video.raw", O_WRONLY | O_CREAT |
					O_TRUNC, 0644);
			break;
		case 'i':
			in_fd = open(optarg, O_RDONLY);
			if (in_fd < 0)
				err(1, "open(%s)", optarg);
			break;
		case 'l':
			pkt_lim = atol(optarg);
			break;
		}
	}

	while (1) {
		printf("off=%6zx", off);
		to_read = sizeof(header) - skip;
		rd = read(in_fd, (void *)&header + skip, to_read);
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
				rd = read(in_fd, &header, 1);
				skipped++;
				off++;
			} while (rd > 0 && header.FA != 0xfa);
			if (rd <= 0)
				break;
			printf(" BAD; skipped: %u\n", skipped + 4 - 1);
			skip = 1;
			continue;
		}

		header.val = ntohl(header.val);

		printf(" hdr=%.8x SEQ=%"PAD"4u/%"PAD"3x%s SY=%u T=%u%u%u%u R=%u%u%u %c%c%c%c",
		       header.val,
		       HEADER_SEQ(&header), HEADER_SEQ(&header),
		       (HEADER_SEQ(&header) & 1) ? " ODD" : "",
		       HEADER_SY(&header),
		       !!HEADER_TY1(&header), !!HEADER_TY2(&header),
		       !!HEADER_TY2(&header), !!HEADER_TY3(&header),
		       !!HEADER_res1(&header), !!HEADER_res2(&header),
		       !!HEADER_res3(&header),
		       HEADER_REAL(&header) ? 'R' : '_',
		       HEADER_AUDIO(&header) ? 'A' : '_',
		       HEADER_SYNC(&header) ? 'S' : '_',
		       HEADER_FINAL(&header) ? 'F' : '_');

		bool old, reset = false;
		to_read = get_data_size(&header, &old);
		printf(" len=%"PAD"3zd %c",
		       to_read, old ? '!' : '_');
		if (to_read) {
			rd = read(in_fd, buf, to_read);
			if (rd < 0) {
				puts("");
				fflush(stdout);
				err(1, "read(data) at 0x%zx", off);
			}
			if (rd != to_read)
				break;
			if (to_read == 180) {
				if (HEADER_SYNC(&header)) {
					printf(" S");
					reset = true;
					synced = true;
				} else if (HEADER_AUDIO(&header)) {
					printf(" A");
					if (synced && audio_fd >= 0)
						write(audio_fd, buf, rd);
				} else {
					printf(" V");
					if (synced && video_fd >= 0 &&
					    vsize + (size_t)rd < sizeof(videobuf))
						memcpy(&videobuf[vsize], buf,
						       rd);
					vsize += rd;
				}
			} else
				printf(" _");

			off += rd;
			printf(" vsize=%"PAD"6u", vsize);
			dump_data_limited("payl", buf, rd, 16);
		}

		puts("");

		if (reset && vsize) {
			if (synced && video_fd >= 0) {
				write(video_fd, videobuf, sizeof(videobuf));
				memset(videobuf, 0, sizeof(videobuf));
			}
			printf("vsize=%u\n", vsize);
			vsize = 0;
		}

		if (pkt++ >= pkt_lim)
			break;
	}

	puts("");

	close(audio_fd);
	close(video_fd);

	return 0;
}

