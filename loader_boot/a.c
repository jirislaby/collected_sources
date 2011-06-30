#include <stdio.h>
#include <stdlib.h>

static long get_file_size(FILE *f)
{
    long where, size;

    /* XXX: on Unix systems, using fstat() probably makes more sense */

    where = ftell(f);
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, where, SEEK_SET);

    return size;
}

int main(void)
{
	unsigned char header[1024];
    FILE *f = fopen("head.bin", "rb");
    if (!f || !(get_file_size(f)) ||
        fread(header, 1, 1024, f) != 1024) {
        fprintf(stderr, "qemu: could not load kernel '%s'\n",
                "");
        exit(1);
    }
puts("OK");
	return 0;
}
