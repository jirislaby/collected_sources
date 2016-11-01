#define _GNU_SOURCE
#include <string.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <linux/if_alg.h>

int main()
{
	struct sockaddr_alg alg = {
		.salg_family = 0x26,
		.salg_type = "hash",
		.salg_feat = 0xf,
		.salg_mask = 0x5,
		.salg_name = "digest_null",
	};
	int sock, sock2;

	sock = socket(AF_ALG, SOCK_SEQPACKET, 0);
        bind(sock, (struct sockaddr *)&alg, sizeof(alg));
        sock2 = accept(sock, NULL, NULL);
        setsockopt(sock, SOL_ALG, ALG_SET_KEY, "\x9b\xca", 2);
        accept(sock2, NULL, NULL);
        return 0;
}

