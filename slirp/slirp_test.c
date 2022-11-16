#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/udp.h>

#include <libslirp.h>

static uint16_t my_cksum(void *_addr, unsigned count)
{
	uint16_t *addr = _addr;
	long sum = 0;

	while (count > 1)  {
		sum += *addr++;
		count -= 2;
	}

	if (count > 0)
		sum += *(unsigned char *)addr;

	while (sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	return ~sum;
}

struct ip6_pseudohdr {
	struct in6_addr ih_src; /* source internet address */
	struct in6_addr ih_dst; /* destination internet address */
	uint32_t ih_pl; /* upper-layer packet length */
	uint16_t ih_zero_hi; /* zero */
	uint8_t ih_zero_lo; /* zero */
	uint8_t ih_nh; /* next header */
};

static uint16_t my_cksum6(struct ip6_hdr *ip6)
{
	struct ip6_pseudohdr *ih = (void *)ip6;
	struct ip6_hdr saved = *ip6;
	uint16_t csum;

	ih->ih_src = saved.ip6_src;
	ih->ih_dst = saved.ip6_dst;
	ih->ih_pl = htonl((uint32_t)ntohs(saved.ip6_plen));
	ih->ih_zero_hi = 0;
	ih->ih_zero_lo = 0;
	ih->ih_nh = saved.ip6_nxt;

	csum = my_cksum(ip6, (int)sizeof(struct ip6_pseudohdr) + ntohl(ih->ih_pl));

	*ip6 = saved;

	return csum;
}

static void register_poll_fd(int, void *)
{
}

static void unregister_poll_fd(int, void *)
{
}

static void *timer_new_opaque(SlirpTimerId, void *, void *)
{
	static char a;

	return &a;
}

static void timer_free(void *, void *)
{
}

static void timer_mod(void *, int64_t, void *)
{
}

static int64_t clock_get_ns(void *)
{
	return 123;
}

#define offsetof(TYPE, MEMBER)  __builtin_offsetof(TYPE, MEMBER)

int main(/*int argc, char **argv*/)
{
	SlirpConfig cfg = {
		.version = SLIRP_MAJOR_VERSION,
		.in_enabled = 1,
		.in6_enabled = 1,
		.vnameserver = { .s_addr = htonl(0x0a000203) },
	};
	SlirpCb cb = {
		.register_poll_fd = register_poll_fd,
		.unregister_poll_fd = unregister_poll_fd,
		.timer_new_opaque = timer_new_opaque,
		.timer_free = timer_free,
		.timer_mod = timer_mod,
		.clock_get_ns = clock_get_ns,
	};
	Slirp *s;
	struct pkt4 {
		struct ether_header eth;
		struct ip ip;
		struct udphdr udp;
		unsigned char data[16];
	} __attribute__((packed)) pkt4 = {
		.eth = {
			.ether_type = htons(ETHERTYPE_IP),
		},
		.ip = {
			.ip_v = IPVERSION,
			.ip_hl = sizeof(struct ip) / 4,
			.ip_len = htons(sizeof(pkt4) - offsetof(struct pkt4, ip)),
			.ip_ttl = 64,
			.ip_p = IPPROTO_UDP,
			.ip_dst = { .s_addr = htonl(0x0a000203) },
		},
		.udp = {
			.uh_dport = htons(53),
			.uh_ulen = htons(sizeof(pkt4) - offsetof(struct pkt4, udp)),
		},
	};
	struct pkt6 {
		struct ether_header eth;
		struct ip6_hdr ip6;
		struct udphdr udp;
		unsigned char data[16];
	} __attribute__((packed)) pkt6 = {
		.eth = {
			.ether_type = htons(ETHERTYPE_IPV6),
		},
		.ip6 = {
			.ip6_flow = 6 << 4,
			.ip6_hlim = 64,
			.ip6_plen = htons(sizeof(pkt6) - offsetof(struct pkt6, udp)),
			.ip6_nxt = IPPROTO_UDP,
		},
		.udp = {
			.uh_dport = htons(53),
			.uh_ulen = htons(sizeof(pkt6) - offsetof(struct pkt6, udp)),
		},
	};

	pkt4.ip.ip_sum = my_cksum(&pkt4.ip, sizeof(pkt4.ip));
	pkt6.udp.uh_sum = my_cksum6(&pkt6.ip6);

	s = slirp_new(&cfg, &cb, NULL);
	slirp_input(s, (uint8_t *)&pkt4, sizeof(pkt4));
	slirp_input(s, (uint8_t *)&pkt6, sizeof(pkt6));
	slirp_cleanup(s);

	return 0;
}

