/*
 *		  LCD - e x a m p l e
 *
 * Description : Extended functions used for 4-bit communication with LCD
 *	       between ATmega128 and 4 lines LCD MC4004
 *
 * Author: Jiri Slaby
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *		 ATmega128 14,7456 MHz
 *
 * Compiler : avr-gcc 4.1.0
 */
#include <avr/io.h>
#include <util/delay.h>

#include "lcd.h"
#include "shift.h"

static uint8_t text[] = "";

static void hw_init(void)
{      
	SHIFT_DDR |= _BV(SHIFT_LED_SET_DDR);	/* parallel write -> output pin */
	SHIFT_DDR |= _BV(SHIFT_CLK_DDR);	/* serial clock -> output pin */
	SHIFT_DDR |= _BV(SHIFT_OUT_DDR);	/* serial data stream -> output pin */ 
	SHIFT_DDR |= _BV(SHIFT_LCD_SET_DDR);	/* parallel write -> output pin */

	LCD_init();			     /* Init LCD display */
}

int main(void)
{
	hw_init();

	LCD_wr_cgram_all(1);

	LCD_cursor_yx(1,1);		     /* Display first line */
	LCD_puts(text,1);

	while (1)
		__asm__ __volatile__("nop"); /* don't optimise it away */

	return 0;
}
