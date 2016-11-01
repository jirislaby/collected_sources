#include <string.h>
#include <stdint.h>
#include <sys/syscall.h>
#include <unistd.h>

int r[12];

int main()
{
        memset(r, -1, sizeof(r));
        r[0] = syscall(SYS_mmap, 0x20000000ul, 0x32000ul, 0x3ul, 0x32ul, 0xfffffffffffffffful, 0x0ul);
        r[1] = syscall(SYS_socket, 0x26ul, 0x5ul, 0x0ul, 0, 0, 0);
        *(uint16_t*)0x20011cbf = (uint16_t)0x26;
        memcpy((void*)0x20011cc1, "\x68\x61\x73\x68\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 14);
        *(uint32_t*)0x20011ccf = (uint32_t)0xf;
        *(uint32_t*)0x20011cd3 = (uint32_t)0x5;
        memcpy((void*)0x20011cd7, "\x64\x69\x67\x65\x73\x74\x5f\x6e\x75\x6c\x6c\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 64);
        r[7] = syscall(SYS_bind, r[1], 0x20011cbful, 0x58ul, 0, 0, 0);
        r[8] = syscall(SYS_accept4, r[1], 0x0ul, 0x20010e9cul, 0x80000ul, 0, 0);  
        memcpy((void*)0x20027000, "\x9b\xca", 2);
        r[10] = syscall(SYS_setsockopt, r[1], 0x117ul, 0x1ul, 0x20027000ul, 0x2ul, 0);
        r[11] = syscall(SYS_accept, r[8], 0x0ul, 0x2002fffeul, 0, 0, 0);
        return 0;
}

