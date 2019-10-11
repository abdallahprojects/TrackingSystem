/*
 * TRACKER.c
 *
 * Created: 10/6/2019 12:00:38 PM
 * Author : asere
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <SWuart.h>
int main(void)
{
	// flip PA2 status
    /* Replace with your application code */
	DDRA |= 0xFF;
	Init_SWuart(57600);
    while (1) 
    {
    	_delay_ms(1000);
		Tx_SWuart_Str("Hello world !!");
		//Tx_SWuart(0x61);
    }
}
