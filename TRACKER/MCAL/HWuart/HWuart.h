/*
 * HWuart.h
 *
 *  Created on: Oct 10, 2019
 *      Author: asere
 */

#ifndef MCAL_HWUART_HWUART_H_
#define MCAL_HWUART_HWUART_H_

#define RX_HWBUFF_SIZE	20
#define TX_HWBUFF_SIZE	150

#define EnableTxInterrupt()		UCSRB |= (1<<UDRIE)

#define DisableTxInterrupt()	UCSRB &= ~(1<<UDRIE)

#define EnableRxInterrupt()		UCSRB |= (1 << RXCIE)

#define DisableRxInterrupt()	UCSRB &= ~(1<<RXCIE)

#define ENABLE_HW_PRINTF() HWuart_EnablePrintf()

typedef enum HWuart_Status_E{
	HWuart_Rx_No_Data,
	HWuart_Rx_OK,
	HWuart_Rx_OK_Again,
	HWuart_Rx_OverFlow,
	HWuart_Tx_Ok,
	HWuart_Tx_BufferFull,
	HWuart_TX_Buffered,
	HWuart_Tx_OverFlow,
	HWuart_MEM_ERR
	}HWuart_Status_T;

typedef enum HWuart_state_E{
		HWuart_transmitting,
		HWuart_trans_in_byte,
		HWuart_trans_overflow,
		HWuart_idle
	}HWuart_state_T;

void HWuart_Init(long USART_BAUDRATE);
HWuart_Status_T HWuart_Tx(uint8_t byte);
HWuart_Status_T HWuart_Rx(uint8_t * byte);
void HWuart_EnablePrintf(void);

#endif /* MCAL_HWUART_HWUART_H_ */
