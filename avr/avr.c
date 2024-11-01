#define F_CPU 8000000UL

#include <avr/io.h>

#include <util/delay.h>

#define BIT(x)	(1U << (x))

int main()
{
	DDRB |= (1 << PB0);

	while (1) {
		PORTB |= BIT(PB0);
		_delay_ms(1000);
		PORTB &= ~BIT(PB0);
		_delay_ms(1000);
	}

	return 0;
}
