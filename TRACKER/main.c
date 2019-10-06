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
	INIT_uart(9600);
    while (1) 
    {
		
    }
}
