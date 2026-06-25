#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#define PORT_LED1	GPIOC
#define PIN_LED1	GPIO13
#define LITTLE_BIT	200000

int main(void) {
	rcc_periph_clock_enable(RCC_GPIOC);
#if defined(STM32F1) /* F1 is a precious snowflake */
	gpio_set_mode(PORT_LED1, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, PIN_LED1);
#else /* everyone else is sane */
        gpio_mode_setup(PORT_LED1, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_LED1);
#endif
	gpio_set(PORT_LED1, PIN_LED1);
	while(1) {
		/* wait a little bit */
		for (int i = 0; i < LITTLE_BIT; i++) {
			__asm__("nop");
		}
		gpio_toggle(PORT_LED1, PIN_LED1);
	}
}
