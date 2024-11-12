#define F_CPU 8000000UL

#include <avr/io.h>

#include <util/delay.h>

int main()
{
	DDRA |= _BV(PA0);

	while (1) {
		PORTA |= _BV(PA0);
		//_delay_ms(1000);
		PORTA &= ~_BV(PA0);
		//_delay_ms(1000);
	}

	return 0;
}
