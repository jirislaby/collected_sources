#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>

//#define OVERFLOWS_NEEDED 143  // ~10 min @ 8 MHz / 1024 prescaler
#define OVERFLOWS_NEEDED	1
#define EEPROM_METADATA_ADDR	0
#define EEPROM_DATA_START	2
#define EEPROM_SIZE		512
#define MAX_SAMPLES		((EEPROM_SIZE - EEPROM_DATA_START) / 2)

#define SERIAL
#define BAUD			9600U

volatile uint16_t overflow_count = 0;

ISR(TIMER1_OVF_vect)
{
	overflow_count++;
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

static void setup_adc()
{
	ADMUX = _BV(REFS0) | _BV(MUX0); // AVcc reference, ADC0
	ADCSRA = _BV(ADEN) | _BV(ADPS1) | _BV(ADPS0); // Enable ADC, prescaler /8
}

static uint16_t read_adc()
{
	PORTA |= _BV(PA7);
	ADCSRA |= _BV(ADSC);

	while (ADCSRA & _BV(ADSC))
		;

	PORTA &= ~_BV(PA7);

	return ADCW;
}

static void setup_timer1()
{
	TCCR1A = 0;
	//TCCR1B = _BV(CS12) | _BV(CS10);	// Prescaler 1024
	//TCCR1B = _BV(CS12);	// Prescaler 256
	TCCR1B = _BV(CS11) | _BV(CS10);	// Prescaler 64
	TIMSK = _BV(TOIE1);		// Overflow interrupt
}

static uint8_t val_array[256];
static uint8_t val_idx;

static void log_val(uint8_t value)
{
	val_array[val_idx++] = value;
	return;

	uint16_t index = eeprom_read_word((uint16_t *)EEPROM_METADATA_ADDR);
	if (index >= MAX_SAMPLES) {
		index = 0;
	}

	uint16_t address = EEPROM_DATA_START + index * 2;
	eeprom_write_word((uint16_t*)address, value);
	eeprom_write_word((uint16_t*)EEPROM_METADATA_ADDR, index + 1);
}

static void do_sleep()
{
	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_enable();
	sleep_cpu();
	sleep_disable();
}

#define VALUE_OFFSET	0x140

int main(void)
{
	DDRA |= _BV(PA0) | _BV(PA7);
	PORTA = _BV(0);
	ACSR |= _BV(ACD);               // Disable analog comparator

	cli();

	setup_uart();
	setup_timer1();
	setup_adc();

	sei();

	while (1) {
		do_sleep();

		if (overflow_count >= OVERFLOWS_NEEDED) {
			overflow_count = 0;

			uint16_t value = read_adc();
			if (value < 0x200)
				PORTA |= _BV(PA0);
			else
				PORTA &= ~_BV(PA0);
			log_val(value >> 2);
			//send_serial_byte(value >> 2);
			send_serial_byte(val_idx);
			for (unsigned int i = 0; i < sizeof(val_array); i++)
				send_serial_byte(val_array[i]);

		}
	}
}
