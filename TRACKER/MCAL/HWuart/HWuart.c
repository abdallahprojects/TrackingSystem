/*
 * HWuart.c
 *
 *  Created on: Oct 10, 2019
 *      Author: asere
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include <stdio.h>

#include "HWuart.h"

#define BAUD_CONV(BAUD) (((F_CPU / (BAUD * 16UL))) - 1)


static volatile uint8_t RxBuffer[RX_BUFF_SIZE+1];
static volatile uint8_t TxBuffer[TX_BUFF_SIZE+1];
static volatile uint16_t RxBufferHead,RxBufferTail;
static volatile uint16_t TxBufferHead,TxBufferTail;
static FILE uart_str;

static volatile HWuart_Status_T RxStatus = HWuart_Rx_No_Data;

int HWuartWrap_Tx(char ch,FILE * stream);


void HWuart_Init(long USART_BAUDRATE)
{
	// Init UART PINS
	DDRD |= (1<<PD1);  // Tx pin as output
	DDRD &= ~(1<<PD0);	// Rx pin as input
	//PORTD |= (1<<PD0); // pull up the pin
	UCSRB |= (1 << RXEN) | (1 << RXCIE)| (1 << TXEN);/* Turn on the transmission and reception */
	UCSRC |= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);/* Use 8-bit character sizes */

	UBRRL = (uint8_t)BAUD_CONV(USART_BAUDRATE);		/* Load lower 8-bits of the baud rate */
	UBRRH = (uint8_t)(BAUD_CONV(USART_BAUDRATE) >> 8);	/* Load upper 8-bits */

	uart_str = FDEV_SETUP_STREAM(HWuartWrap_Tx, NULL, _FDEV_SETUP_RW);


}
void HWuart_EnablePrintf(void)
{
	stdout = &uart_str;
}

ISR(USART_RXC_vect)
{
	RxBuffer[RxBufferHead] = UDR;
	RxBufferHead = (RxBufferHead == RX_BUFF_SIZE)? 0:(RxBufferHead+1);
	if((RxBufferHead+1 == RxBufferTail)||((RxBufferHead == RX_BUFF_SIZE) && (RxBufferTail == 0)))
	{
		RxStatus = HWuart_Rx_OverFlow;
	}
}

ISR(USART_UDRE_vect)
{
	// Tx Interrupt
	// Read to load next byte in UDR register
	if (TxBufferTail == TxBufferHead){ // if buffer is empty
		DisableTxInterrupt();
	}else{
	UDR = TxBuffer[TxBufferTail++];
	if(TxBufferTail == TX_BUFF_SIZE+1)
		TxBufferTail = 0;
	}

}
HWuart_Status_T HWuart_Tx(uint8_t byte)
{
	HWuart_Status_T ret;

		if((TxBufferHead+1 == TxBufferTail)||((TxBufferHead == TX_BUFF_SIZE) && (TxBufferTail == 0)))
			{
				//cant store more bytes
				ret = HWuart_Tx_OverFlow;
			}else{
				if(UCSRA & (1<<UDRE)) // Transmit is Idle
				{
					TxBuffer[TxBufferHead] = byte;
					TxBufferHead = (TxBufferHead == TX_BUFF_SIZE)? 0:(TxBufferHead+1);
					EnableTxInterrupt();
					ret = HWuart_Tx_Ok;
				}else{
					//Since UART Tx is busy fill in the buffer and wait to be handled in interrupt
					// but make sure not to be interrupted while filling in the buffer
					DisableTxInterrupt();
					TxBuffer[TxBufferHead] = byte;
					TxBufferHead = (TxBufferHead == TX_BUFF_SIZE)? 0:(TxBufferHead+1);
					EnableTxInterrupt();
					ret = HWuart_TX_Buffered;
				}
			}
	return ret;
}
HWuart_Status_T HWuart_Rx(uint8_t * byte)
{
	HWuart_Status_T ret;
	if(RxStatus != HWuart_Rx_OverFlow)
	{
		 if (RxBufferTail == RxBufferHead){
				ret =  HWuart_Rx_No_Data;
		}else
		{
			DisableRxInterrupt();
			*byte = RxBuffer[RxBufferTail++];
			if(RxBufferTail == RX_BUFF_SIZE+1)
				RxBufferTail = 0;

			EnableRxInterrupt();
			if (RxBufferHead == RxBufferTail){
						ret =  HWuart_Rx_OK;
			}else{
				ret = HWuart_Rx_OK_Again;
			}
		}

	}else{
		// reset the SW
		ret = HWuart_Rx_OverFlow;
	}
	return ret;
}

int HWuartWrap_Tx(char ch,FILE * stream)
{
	HWuart_Tx(ch);
	return 0;
}

