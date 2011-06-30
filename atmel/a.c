#include <stdio.h>

#define nop() asm("nop")

static void delay()
{
	unsigned long a;

	for (a = 0; a < 100000; a++)
		nop();
}

static void itoa(char *a, unsigned int i)
{
	unsigned int b;

	for (b = 0; b < 4; b++) {
		a[3 - b] = i % 10 + '0';
		i /= 10;
	}
	a[4] = 0;
}

int main()
{
        static unsigned int cnt;
        static unsigned char cnt1;
        char buf[5];

        while (1) {
		delay();
                if (!cnt1++) {
                	itoa(buf, cnt = (cnt + 1) % 10000);
			printf("%s/%.4u\n", buf, cnt);
		}
        }

        return 0;
}

