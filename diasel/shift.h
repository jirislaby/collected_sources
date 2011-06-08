#ifndef _SHIFT_H
#define _SHIFT_H

#include <avr/io.h>

#define SHIFT_IO_DELAY        1

#define SHIFT_PORT            PORTD		/* port for shift registers */
#define SHIFT_PIN             PIND		/* input pin from 74HC165 */
#define SHIFT_DDR             DDRD		/* pin direction bit */

/* Definitions for IC 74HC165 parallel to serial convertor */
#define SHIFT_IN_LOAD         PORTD2		/* shift/!load */
#define SHIFT_IN              PORTD4		/* data */
#define SHIFT_CLK             PORTD7		/* serial clock */

/* Definitions for IC 74HC595 serial to parallel convertor */
#define SHIFT_LCD_SET         PORTD1		/* parallel write clock (storage register clock) */
#define SHIFT_LED_SET         PORTD5		/* parallel write clock (storage register clock) */
#define SHIFT_OUT             PORTD6		/* serial data */

#define SHIFT_IN_LOAD_DDR     DDD2		/* pin direction bit */
#define SHIFT_IN_DDR          DDD4		/* pin direction bit */
#define SHIFT_CLK_DDR         DDD7		/* pin direction bit */

#define SHIFT_LCD_SET_DDR     DDD1		/* pin direction bit */
#define SHIFT_LED_SET_DDR     DDD5		/* pin direction bit */
#define SHIFT_OUT_DDR         DDD6		/* pin direction bit */

#endif

