#include <avr/io.h>
#include <avr/interrupt.h>

#define sbit(port, bit, value) do {				\
	(port) = ((port) & ~_BV(bit)) | (!!(value) << (bit));	\
} while (0)
#define sbi(port, bit) sbit(port, bit, 1)
#define cbi(port, bit) sbit(port, bit, 0)
#define gbi(port, bit) (((port) >> (bit)) & 1)
#define nop()	asm volatile("nop")
#define mb()	asm volatile("" : : : "memory")

#define setHL(reg, value) do {				\
	unsigned long __flags = local_irq_save();	\
	reg ## H = (unsigned char)((value) >> 8);	\
	reg ## L = (unsigned char)(value);		\
	local_irq_restore(__flags);			\
} while (0)

static inline unsigned char local_irq_save(void)
{
	unsigned char flags = SREG;
	cli();
	return flags;
}

static inline void local_irq_restore(unsigned char flags)
{
	SREG = flags;
}

static struct clock {
	unsigned char h;
	unsigned char m;
	unsigned char s;
} now;

static unsigned char byte;

static void ctoa(char *a)
{
	unsigned char b, c, i;
	char *tmp = &a[3];

	for (b = 2; b >= 1; b--) {
		i = (b == 2) ? now.m : now.h;
		for (c = 0; c < 2; c++) {
			*tmp-- = i % 10 + '0';
			i /= 10;
		}
	}
	a[4] = 0;
}

static unsigned char nums1[] = {
	0x00, 0x00, /* 0 */
	0x48, 0x40, /* 1 */
	0x70, 0x70, /* 2 */
	0x68, 0x70, /* 3 */
};

static unsigned char nums24[] = {
	0x07, 0x0b, /* 0 */
	0x06, 0x00, /* 1 */
	0x05, 0x0e, /* 2 */
	0x07, 0x06, /* 3 */
	0x06, 0x05, /* 4 */
	0x03, 0x07, /* 5 */
	0x03, 0x0f, /* 6 */
	0x07, 0x00, /* 7 */
	0x07, 0x0f, /* 8 */
	0x07, 0x07  /* 9 */
};

static unsigned char nums3[] = {
	0x58, 0x70, /* 0 */
	0x00, 0x30, /* 1 */
	0x38, 0x60, /* 2 */
	0x30, 0x70, /* 3 */
	0x60, 0x30, /* 4 */
	0x70, 0x50, /* 5 */
	0x78, 0x50, /* 6 */
	0x00, 0x70, /* 7 */
	0x78, 0x70, /* 8 */
	0x70, 0x70  /* 9 */
};

ISR(TIMER0_OVF_vect)
{
	static unsigned char cyc;
	unsigned char num13, cyc2;

	cyc = (cyc + 1) & 3;
	cyc1 = cyc & 1;
	cyc2 = cyc & 2;
	num13 = (cyc2 ? now.s : now.m) / 10 * 2;
	num13 = (cyc2 ? nums3 : nums1)[num13 + cyc1];
	PORTD &= ~0x3f;
	PORTD |= _BV(cyc + 2) | ((num13 & 0x60) >> 5);

	PORTB = nums24[((cyc2 ? now.s : now.m) % 10) * 2 + cyc1];
	PORTB |= num13 & 0x1f;
}

ISR(TIMER1_COMPA_vect)
{
	static unsigned char on;
	char buf[5];

	sbit(PORTD, PD6, on ^= 1);

	if (on) {
		byte++;
		now.s = (now.s + 1) % 60;
		if (!now.s) {
			now.m = (now.m + 1) % 60;
			if (!now.m)
				now.h = (now.h + 1) % 24;
		}
		ctoa(buf);
	}
}

static inline void sleep(void)
{
	cbi(MCUCR, SM0);
	cbi(MCUCR, SM1);
	mb();
	sbi(MCUCR, SE);
}

#define BAUD	9600

int main()
{
	DDRB = 0x1f; /* display cathode */

//	sbi(DDRD, PD2); /* USART internal clock */

	DDRD = 0x3f; /* display anodes */
	sbi(DDRD, PD6); /* LED -- temporary */

	TCCR0A = 0;
	TCCR0B = _BV(CS01) | _BV(CS00);

	OCR1A = 4500; /* 9216000 MHz / 1024 / 2 = 4500 ~ 0.5 s */
	TCCR1A = 0;
	TCCR1B = _BV(WGM12) | _BV(CS12) | _BV(CS10);

	TIMSK |= _BV(OCIE1A) | _BV(TOIE0);

/*	setHL(UBRR, 9216000 / 2 / BAUD - 1);
	UCSRB = _BV(TXEN);
	UCSRC = _BV(UMSEL) | _BV(UCSZ0) | _BV(UCSZ1);*/

	sei();

	while (1) {
		sleep();
/*		while (!gbi(UCSRA, UDRE))
			nop();
		UDR = byte;*/
	}

	return 0;
}
