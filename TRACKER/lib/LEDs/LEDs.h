/*
 * LEDs.h
 * This file provide controls for four LEDs connected to PA2 - PA1 - PD2 - PD3
 * as LED1 - LED2 - LED3 - LED4
 * Created: 11/2/2019 12:09:15 PM
 *  Author: asere
 */ 


#ifndef LEDS_H_
#define LEDS_H_

// Configuration part 
#define LED1	PORTA,2
#define LED2	PORTA,1
#define LED3	PORTD,2
#define LED4	PORTD,3

#define LEDS_Init()		LED_INIT(LED1);			\
						LED_INIT(LED2);			\
						LED_INIT(LED3);			\
						LED_INIT(LED4);			\
						LED_OFF(LED1);			\
						LED_OFF(LED2);			\
						LED_OFF(LED3);			\
						LED_OFF(LED4)

// END of configuration 



#define _LED_INIT(PORT,PIN)	*(&PORT-1) |= (1<<PIN)
#define _LED_ON(PORT,PIN)	PORT |= (1<<PIN)
#define _LED_OFF(PORT,PIN)	PORT &= ~(1<<PIN)
#define _LED_FLIP(PORT,PIN)	PORT ^= (1<<PIN)

#define LED_INIT(LED)	_LED_INIT(LED)
#define LED_ON(LED)		_LED_ON(LED)
#define LED_OFF(LED)	_LED_OFF(LED)
#define LED_FLIP(LED)	_LED_FLIP(LED)

#endif /* LEDS_H_ */
