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
	Init_SWuart(9600);
    while (1) 
    {
		uint8_t v;
		Status_T st;
    	_delay_ms(1000);
		st = Rx_SWuart(&v);
		while(st == SWuart_Rx_OK || st == SWuart_Rx_OK_Again){
			Tx_SWuart(v);
			st = Rx_SWuart(&v);
		}
		//Tx_SWuart(0x61);
    }
}
