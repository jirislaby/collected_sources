#include <stdio.h>

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

static unsigned char byte;

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
	}
}


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

/*00000226 <main>:
	sbi(MCUCR, SE);
}*/

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

/*		ctoa(buf);
	}*/
}

static inline void sleep(void)
{
	cbi(MCUCR, SM0);
	cbi(MCUCR, SM1);
	mb();
}
