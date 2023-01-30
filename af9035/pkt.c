#include <assert.h>
#include <err.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef USE_PCAP
#include <pcap/pcap.h>
#else
#include "xxx.h"
#endif

struct usb_pkt {
	uint64_t id;
	uint8_t type;
	uint8_t ttype;
	uint8_t ep_dir;
	uint8_t dev;
	uint16_t bus;
	uint8_t setup_req;
	uint8_t data_pres;
	uint64_t sec;
	uint32_t usec;
	uint32_t stat;
	uint32_t ulen;
	uint32_t dlen;
	uint32_t iso_err;
	uint32_t isoc_nr;
	uint32_t intval;
	uint32_t start_frm;
	uint32_t flags_copy;
	uint32_t isoc_nr2;
	unsigned char data[];
} __attribute__((packed));

struct isoc_desc {
	int32_t stat;
	uint32_t off;
	uint32_t len;
	uint32_t pad;
} __attribute__((packed));

struct af9035 {
	uint8_t len;
	union {
		struct {
			uint8_t mbox;
			uint8_t cmd;
			uint8_t seq;
			unsigned char data[];
		} wr;
		struct {
			uint8_t seq;
			uint8_t sta;
			unsigned char data[];
		} rd;
	};
} __attribute__((packed));

#define ARRAY_SIZE(x)	(sizeof(x) / sizeof(*(x)))
#define min(a, b)	((a) < (b) ? (a) : (b))

#define CMD_MEM_RD                  0x00
#define CMD_MEM_WR                  0x01
#define CMD_I2C_RD                  0x02
#define CMD_I2C_WR                  0x03
#define CMD_IR_GET                  0x18
#define CMD_FW_DL                   0x21
#define CMD_FW_QUERYINFO            0x22
#define CMD_FW_BOOT                 0x23
#define CMD_FW_DL_BEGIN             0x24
#define CMD_FW_DL_END               0x25
#define CMD_FW_SCATTER_WR           0x29
#define CMD_GENERIC_I2C_RD          0x2a
#define CMD_GENERIC_I2C_WR          0x2b

static const struct {
	uint8_t cmd;
	const char *desc;
} cmds[] = {
	{ CMD_MEM_RD        , "MEMRD" },
	{ CMD_MEM_WR        , "MEMWR" },
	{ CMD_I2C_RD        , "I2CRD" },
	{ CMD_I2C_WR        , "I2CWR" },
	{ CMD_IR_GET        , "IRGET" },
	{ CMD_FW_DL         , "FWDL" },
	{ CMD_FW_QUERYINFO  , "QRYFW" },
	{ CMD_FW_BOOT       , "BOOT" },
	{ CMD_FW_DL_BEGIN   , "DLBEG" },
	{ CMD_FW_DL_END     , "DLEND" },
	{ CMD_FW_SCATTER_WR , "SCWR" },
	{ CMD_GENERIC_I2C_RD, "GENRD" },
	{ CMD_GENERIC_I2C_WR, "GENWR" },
};

static void dump_data(const uint8_t *data, unsigned len)
{
	printf(" data=");
	for (unsigned a = 0; a < len; a++)
		printf(" %.2x", data[a]);
}

static void dump_data_limited(const uint8_t *data, unsigned len, unsigned limit)
{
	dump_data(data, min(len, limit));
	if (len > limit)
		printf(" (%u more)", len - limit);
}

static const char *get_cmd_desc(uint8_t cmd)
{
	for (unsigned a = 0; a < ARRAY_SIZE(cmds); a++)
		if (cmds[a].cmd == cmd)
			return cmds[a].desc;

	return "UNK";
}

static const char *get_reg_name(uint8_t mbox, const uint8_t reg_arr[static 2])
{
	static char buf[32];
	const char *reg_str;
	uint32_t reg = mbox;
	reg <<= 8;
	reg |= reg_arr[0];
	reg <<= 8;
	reg |= reg_arr[1];

	switch (reg) {
	case 0x1222:
		return "CHIP_VER";
	case 0x384f:
		return "PRECHIP_VER";
	case 0x417f:
		return "PRE_EN_CLK?";
	case 0xd800:
		return "DEMOD_CLOCK";
	case 0xd81a:
		return "EN_CLK";
	case 0xf103:
		return "I2Cm13_CLK_SPD";
	case 0xf6a7:
		return "I2Cm2_CLK_SPD";
	case 0xd827 ... 0xd829:
	case 0xd830 ... 0xd832:
	case 0xd8fd:
		reg_str = "DEM_AF9033";
		break;
	case 0xd8b3 ... 0xd8e1:
		reg_str = "TUN_MXL";
		break;
	case 0xdd11:
		return "dual_mode??";
	case 0xdd13:
		return "af9035_init";
	case 0xdd88:
		return "frame_sizeL";
	case 0xdd0c:
		return "packet_size";
	case 0x800000 ... 0x80ffff:
		if (reg == 0x800000 || reg == 0x80004c || reg == 0x80fb24)
			reg_str = "SLEEP";
		else
			reg_str = "TUN_IT9135";
		break;
	default:
		reg_str = "UNK";
		break;
	}

	sprintf(buf, "%s(%.6x)", reg_str, reg);

	return buf;
}

static bool handle_wr_cmd(const struct af9035 *af, const uint8_t *data,
		unsigned data_len)
{
	switch (af->wr.cmd) {
	case CMD_MEM_RD:
		printf(" len=%2u reg=%-20s", data[0],
				get_reg_name(af->wr.mbox, &data[4]));
		return true;
	case CMD_MEM_WR:
		printf(" len=%2u reg=%-20s", data[0],
				get_reg_name(af->wr.mbox, &data[4]));
		dump_data(&data[6], data[0]);
		return true;

	case CMD_GENERIC_I2C_WR:
		printf(" len=%2u bus=%.2x addr=%.2x", data[0], data[1],
				data[2]);
		dump_data(&data[3], data[0]);
		return true;

	case CMD_FW_QUERYINFO:
		printf(" start=%u", data[0]);
		return true;

	case CMD_FW_DL:
		printf(" len=%u", data_len);
		dump_data_limited(data, data_len, 20);
		return true;

	case CMD_FW_DL_BEGIN:
	case CMD_FW_DL_END:
		return true;
	};

	return false;
}

static void handle_af9035(uint32_t sec, uint32_t usec, const struct af9035 *af,
			  bool in)
{
	const uint8_t *raw = (const void *)af;
	uint16_t csum = raw[af->len - 1];
	unsigned int start = 3;
	bool do_dump_data = true;

	csum <<= 8;
	csum |= raw[af->len];

	printf("\t%4u.%.4u BULK %s len=%2u csum=%4x", sec, usec / 100,
	       in ? " IN" : "OUT", af->len, csum);

	if (in) {
		printf(" seq=%3u sta=%.2x", af->rd.seq, af->rd.sta);
	} else {
		start++;
		printf(" seq=%3u mbox=%.2x cmd=%5s", af->wr.seq, af->wr.mbox,
				get_cmd_desc(af->wr.cmd));
		do_dump_data = !handle_wr_cmd(af, &raw[start],
				af->len - 1 - start);
	}

	if (do_dump_data)
		dump_data(&raw[start], af->len - 1 - start);

	puts("");
}

static void handle_bulk(const struct usb_pkt *pkt, uint32_t sec)
{
	bool in = pkt->ep_dir & (1U << 7);

	if (pkt->dlen)
		handle_af9035(sec, pkt->usec, (const void *)pkt->data, in);
}

static void handle_single_isoc(const void *isoc_data, uint32_t len)
{
	dump_data_limited(isoc_data, len, 20);
	write(2, isoc_data, len);
}

static void handle_isoc(const struct usb_pkt *pkt, uint32_t sec)
{
	bool in = pkt->ep_dir & (1U << 7);

	printf("\t%4u.%.4u ISOC %s isoc_nr=%3u\n",
			sec, pkt->usec / 100, in ? " IN" : "OUT",
			pkt->isoc_nr);

	if (pkt->type == 'S')
		return;

	const struct isoc_desc *isoc = (const void *)pkt->data;
	const void *isoc_data = isoc + pkt->isoc_nr;
	for (unsigned a = 0; a < pkt->isoc_nr; a++, isoc++) {
		printf("\t\tIDESC %2u[%4d/%5u/%4u]", a, isoc->stat, isoc->off,
		       isoc->len);
		if (isoc->stat == 0)
			handle_single_isoc(isoc_data + isoc->off, isoc->len);
		puts("");
	}
}

static void handle_packet(const struct usb_pkt *pkt)
{
	static uint64_t first_sec;

	if (!first_sec)
		first_sec = pkt->sec;

	printf("%c ttype=%u, dlen=%3u", pkt->type, pkt->ttype, pkt->dlen);
	dump_data_limited(pkt->data, pkt->dlen, 30);
	puts("");

	switch (pkt->ttype) {
	case 0x00:
		handle_isoc(pkt, pkt->sec - first_sec);
		break;
	case 0x03:
		handle_bulk(pkt, pkt->sec - first_sec);
		break;
	}
}

#ifdef USE_PCAP
static void pcap_cb(u_char *, const struct pcap_pkthdr *, const u_char *bytes)
{
	handle_packet((const void *)bytes);
}
#endif

int main(int, char **argv)
{
	assert(sizeof(struct usb_pkt) == 0x40);

#ifdef USE_PCAP
	char errbuf[PCAP_ERRBUF_SIZE];

	if (pcap_init(0, errbuf) < 0)
		errx(1, "pcap_init: %s", errbuf);

	pcap_t *pcap = pcap_open_offline(argv[1], errbuf);
	if (!pcap)
		errx(1, "pcap_open_offline: %s", errbuf);

	if (pcap_loop(pcap, -1, pcap_cb, NULL) == PCAP_ERROR)
		errx(1, "pcap_loop: %s", pcap_geterr(pcap));


	pcap_close(pcap);
#else
	(void)argv;
	for (unsigned a = 0; a < packets_nr; a++)
		handle_packet((const void *)packets[a]);
#endif

	return 0;
}
