/*
 * TRACKER.c
 *
 * Created: 10/6/2019 12:00:38 PM
 * Author : asere
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <SWuart.h>
#include <ublox_6m.h>

int main(void)
{
	// flip PA2 status
    /* Replace with your application code */
	DDRA |= 0xFF;
	Init_SWuart(9600);
    while (1) 
    {
		_delay_ms(1);
    	ublox_cyclic5ms();
		//Tx_SWuart(0x61);
    }
}
