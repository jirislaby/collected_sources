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
#define HEADER_WORDS(hdr)	(((hdr)->val & 0x00f00000) >> 20)
#define HEADER_SEQ_BOT(hdr)	(((hdr)->val & 0x000f0000) >> 16)
#define  HEADER_SEQ_BOT_BITS	4
#define HEADER_SEQ_TOP(hdr)	(((hdr)->val & 0x0000f800) >> 11)
#define HEADER_FLIP(hdr)	(((hdr)->val & 0x00000400) >> 10)
#define HEADER_HISIZE(hdr)	(((hdr)->val & 0x00000300) >>  8)
#define HEADER_REAL(hdr)	(((hdr)->val & 0x00000080) >>  7)
#define HEADER_AUDIO(hdr)	(((hdr)->val & 0x00000040) >>  6)
#define HEADER_SYNC(hdr)	(((hdr)->val & 0x00000020) >>  5)
#define HEADER_unk1(hdr)	(((hdr)->val & 0x00000010) >>  4)
#define HEADER_XXX_MASK(hdr)	(((hdr)->val & 0x000000f0) >>  4)
#define HEADER_SY(hdr)		(((hdr)->val & 0x0000000f) >>  0)

static inline uint16_t HEADER_SEQ(const struct header *hdr)
{
	uint16_t seq = HEADER_SEQ_TOP(hdr);

	seq <<= HEADER_SEQ_BOT_BITS;
	seq |= HEADER_SEQ_BOT(hdr);

	if (seq & 1)
		printf(" ODD SEQ");

	return seq >> 1;
}

static inline unsigned HEADER_SIZE(const struct header *hdr)
{
	return (HEADER_HISIZE(hdr) << 6) | (HEADER_WORDS(hdr) << 2);
}

int main(int argc, char **argv)
{
	struct header header;
	uint8_t buf[184 - sizeof(header)], videobuf[720*576/2*2];
	unsigned int pkt = 0, pkt_lim = UINT_MAX;
	ssize_t rd, to_read;
	size_t off = 0;
	unsigned vsize = 0, asize = 0, ssize = 0;
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
		printf("off=%8zx", off);
		rd = read(in_fd, (void *)&header, sizeof(header));
		if (!rd)
			break;

		if (rd != sizeof(header)) {
			puts("");
			fflush(stdout);
			err(1, "read(header)");
		}

		off += rd;

		if (header.FA != 0xfa) {
			printf(" BAD(%.8x)\n", ntohl(header.val));
			continue;
		}

retry:
		header.val = ntohl(header.val);

		to_read = HEADER_SIZE(&header);
		printf(" hdr=%.8x SEQ=%3u/%2x SY=%u %c%c%c%c%c len=%3zd",
		       header.val,
		       HEADER_SEQ(&header), HEADER_SEQ(&header),
		       HEADER_SY(&header),
		       HEADER_FLIP(&header) ? 'F' : '_',
		       HEADER_REAL(&header) ? 'R' : '_',
		       HEADER_AUDIO(&header) ? 'A' : '_',
		       HEADER_SYNC(&header) ? 'S' : '_',
		       HEADER_unk1(&header) ? 'U' : '_',
		       to_read);

		bool dump_video = false;
		if (to_read) {
			rd = read(in_fd, buf, to_read);
			if (rd < 0) {
				puts("");
				fflush(stdout);
				err(1, "read(data) at 0x%zx", off);
			}
			if (rd != to_read)
				break;

			/* BUG in FW? */
			if (to_read == 4 && buf[0] == 0xfa) {
				printf(" BAD -- retrying\noff=%8zx", off);
				memcpy(&header, buf, sizeof(header));
				goto retry;
			}

			if (HEADER_unk1(&header)) {
				printf(" U");
			} else if (HEADER_SYNC(&header)) {
				ssize += rd;
				printf(" S");
				dump_video = true;
				if (!synced) {
					asize = 0;
					ssize = 0;
					vsize = 0;
				}
				synced = true;
			} else if (HEADER_AUDIO(&header)) {
				printf(" A");
				if (synced && audio_fd >= 0)
					write(audio_fd, buf, rd);
				asize += rd;
			} else if (to_read == 180) {
				printf(" V");
				if (synced && video_fd >= 0 &&
				    vsize + (size_t)rd < sizeof(videobuf))
					memcpy(&videobuf[vsize], buf,
					       rd);
				vsize += rd;
			} else
				printf(" _");

			off += rd;
			printf(" vsize=%6u asize=%8u", vsize, asize);
			dump_data_limited("payl", buf, rd, 12);
		}

		puts("");

		if (dump_video && vsize) {
			if (synced && video_fd >= 0) {
				printf("dumping; ");
				write(video_fd, videobuf, sizeof(videobuf));
				memset(videobuf, 0, sizeof(videobuf));
			}
			printf("vsize=%u ssize=%u\n", vsize, ssize);
			vsize = 0;
			ssize = 0;
		}

		if (pkt++ >= pkt_lim)
			break;
	}

	puts("");

	close(audio_fd);
	close(video_fd);

	return 0;
}

