#include <stdbool.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>

#include <util/twi.h>

#if defined(_AVR_ATmega16A_H_)
#define I2C_IRQ		1

static void set_TWCR(bool initial, bool ack, bool stop)
{
	uint8_t irq_bit = I2C_IRQ ? _BV(TWIE) : 0;
	uint8_t ack_bit = ack ? _BV(TWEA) : 0;
	uint8_t int_bit = initial ? 0 : _BV(TWINT);
	uint8_t stop_bit = stop ? _BV(TWSTO) : 0;

	TWCR = irq_bit | ack_bit | _BV(TWEN) | int_bit | stop_bit;
}

static inline void ack_TWCR(bool ack)
{
	set_TWCR(false, ack, false);
}

static void handle_TW_STATUS()
{
	static uint8_t last_data;
	PORTA ^= _BV(PA0);
	PORTD ^= _BV(6);

	switch (TW_STATUS) {
	case TW_SR_SLA_ACK:
	case TW_SR_ARB_LOST_SLA_ACK:
	case TW_SR_GCALL_ACK:
	case TW_SR_ARB_LOST_GCALL_ACK:
		ack_TWCR(true);
		break;
	case TW_SR_DATA_ACK:
		last_data = TWDR;
		//PORTB = last_data;
		ack_TWCR(true);
		break;
	case TW_SR_STOP:
		ack_TWCR(true);
		break;
	case TW_SR_DATA_NACK:
	case TW_SR_GCALL_DATA_NACK:
		ack_TWCR(false);
		break;

	case TW_ST_SLA_ACK:
	case TW_ST_ARB_LOST_SLA_ACK:
	case TW_ST_DATA_ACK:
		PORTD ^= _BV(0);
		TWDR = ++last_data;
		ack_TWCR(true);
		break;
	case TW_ST_DATA_NACK:
	case TW_ST_LAST_DATA:
		PORTD ^= _BV(1);
		ack_TWCR(true);
		break;

	case TW_BUS_ERROR:
		set_TWCR(false, true, true);
		break;
	case TW_NO_INFO:
		break;

	default:
		PORTD ^= _BV(7);
		PORTB = 0;
		PORTB = 1;
		PORTB = 0;
		for (uint8_t bit = 3; bit < 8; bit++)
			PORTB = !!(TW_STATUS & _BV(bit));
		PORTB = 0;
		PORTB = 1;
		PORTB = 0;
		ack_TWCR(true);
		break;
	}
}

#if I2C_IRQ != 0
ISR(TWI_vect)
{
	handle_TW_STATUS();
}
#endif

int main()
{
	DDRA |= _BV(PA0);
	DDRD = 0xff;

	DDRB = 0xff;
	TWAR = 0x11 << 1;
	TWSR = 0;
	set_TWCR(true, true, false);
	if (I2C_IRQ) {
		set_sleep_mode(SLEEP_MODE_IDLE);
		sei();
	}

	while (1) {
		if (I2C_IRQ) {
			sleep_mode();
			continue;
		}

		while (!(TWCR & _BV(TWINT)))
			;
		handle_TW_STATUS();
	}

	return 0;
}

#else

int main()
{
	return 0;
}

#endif
