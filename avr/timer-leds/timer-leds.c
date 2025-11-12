#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>

#define SECONDS		32U
#define TICKS		(SECONDS / 8)
#define SERIAL
#define BAUD		9600U

volatile uint8_t overflow_count = 0;

ISR(TIMER2_OVF_vect)
{
	overflow_count++;
}

static void setup_timer2()
{
	ASSR = _BV(AS2);
	TCCR2 = _BV(CS22) | _BV(CS21) | _BV(CS20);
	while (ASSR & (_BV(TCN2UB) | _BV(TCR2UB) | _BV(OCR2UB)))
		;
	TIMSK |= _BV(TOIE2);
}

static void __attribute__((noinline)) setup_uart()
{
#ifdef SERIAL
	// 9600 baud @ 8 MHz internal clock
	uint16_t ubrr = (F_CPU / 16 / BAUD) - 1;
	UBRRH = ubrr >> 8;
	UBRRL = ubrr;
	UCSRB = _BV(TXEN); // Enable TX
	UCSRC = _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0); // 8N1
#endif
}

static void send_serial_byte(uint8_t value)
{
#ifdef SERIAL
	while (!(UCSRA & _BV(UDRE)))
		;
	UDR = value;
#else
	(void)value;
#endif
}

static void uart_flush_tx(void)
{
    while (!(UCSRA & _BV(TXC)))
        ;
    UCSRA |= _BV(TXC);
}

static void do_sleep()
{
	set_sleep_mode(SLEEP_MODE_PWR_SAVE);
	sleep_mode();
}

int main(void)
{
	DDRA |= _BV(PA0);
	PORTA = _BV(0);
	ACSR |= _BV(ACD);               // Disable analog comparator

	cli();

	setup_uart();
	setup_timer2();

	send_serial_byte('I');
	send_serial_byte('\n');
	uart_flush_tx();

	sei();

	while (1) {
		do_sleep();

		if (overflow_count >= TICKS) {
			overflow_count = 0;

			PORTA ^= _BV(PA0);

			send_serial_byte('A');
			send_serial_byte('\n');
			uart_flush_tx();
		}
	}
}
