#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>

#if defined(_AVR_ATmega16A_H_)

ISR(SPI_STC_vect)
{
	PORTA ^= _BV(PA0);
	PORTD ^= _BV(6);
	uint8_t data = SPDR;
	SPDR = ++data | 0x80;
	PORTB = data;
}

int main()
{
	DDRA |= _BV(PA0);
	DDRD = 0xff;

	MISO_DDR = _BV(MISO_BIT);
	SPCR = _BV(SPE) | _BV(SPIE);
	SPDR = 0xa0;
	sei();

	while (1) {
		sleep_mode();
	}

	return 0;
}

#else

int main()
{
	return 0;
}

#endif
