/*
 * SIM800L.c
 *
 * Created: 11/2/2019 12:05:26 PM
 *  Author: asere
 */ 
//external includes
#include <avr/io.h>
#include <stdio.h>
#include <stdint.h>
#include <HWuart.h>
#include <LEDs.h>
// internal include
#include "SIM800L.h"
#include "SIM800L_Loc.h"

void SIM800L_Init(void){
	HWuart_Init(19200);
	sim800l.tx = printf;
	sim800l.rxWaitFor = rxWaitFor;
	LEDS_Init();
}

void SIM800L_Cyclic5ms(void)
{
	// Enable HW printf
	ENABLE_HW_PRINTF();
	switch(SIM800_State)
	{
		case SIM800_TestAliv:
			TestOK();
			break;
		case SIM800_NetAtt: // Attach to the network
			NetAtt();
			break;
		case SIM800_RxWaiting:
			waiting();
			break;
		default:
			 __asm__ __volatile__ ("nop" ::);
			//Error();
	}
}


void TestOK(void){
	sim800l.tx("AT\n");
	sim800l.rxWaitFor(COMMAND("OK"),1000,SIM800_NetAtt); // wait for 5 seconds if not return back
}
void NetAtt(void) {
	LED_ON(LED2);
	//LED_ON(LED3);
}
void waiting(void){

		static uint8_t i = 0;
		uint8_t in_ch;
		wait.currentTime++;
		if(wait.currentTime < wait.waitTime)
		{
			while(HWuart_Rx(&in_ch) & (HWuart_Rx_OK|HWuart_Rx_OK_Again))
				{
					if(in_ch == wait.compare_string[i])
					{
						i++;
						}else if(in_ch == wait.compare_string[0]){
							i = 1;
						}else{
							i = 0;
					}
					if(i == wait.string_size -1)
					{
						SIM800_State = wait.next_state;
					}
				}
		}else{
			SIM800_State = wait.prev_state;
			i = 0;
		}
}
/* this interface in uart.waitFor, to wait for input until next state is achieved, otherwise
 * the state machine will jump back to the previous state and try again
 */
void rxWaitFor(char * ch,uint8_t size,uint16_t timeout,SIM800_St_t next_state){
	wait.waitTime = timeout;
	wait.currentTime = 0;
	wait.compare_string = ch;
	wait.next_state = next_state;
	wait.string_size = size;
	wait.prev_state = SIM800_State;
	SIM800_State = SIM800_RxWaiting;
}
