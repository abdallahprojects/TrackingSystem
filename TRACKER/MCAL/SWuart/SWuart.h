/*
 * SWuart.h
 *
 * Created: 10/6/2019 1:07:37 PM
 *  Author: asere
 */ 


#ifndef SWuart_H_
#define SWuart_H_
// Configuration settings
#define RXDDR  DDRD
#define RXPORT PORTD
#define RXPIN PD6
#define TXDDR  DDRD
#define TXPORT PORTD
#define TXPIN PD7
#define STOP_BYTES 1

///
#define RX_TIMER_COMP			0x0007
#define Stop_Timer1 			TCCR1B &= ~(1<<CS11); 	\
								TCCR1B &= ~(1<<CS10); 	\
								TCCR1B &= ~(1<<CS12)

#define Start_Timer1 			TCCR1B |= (1<<CS11); 	\
								TCCR1B &= ~(1<<CS10); 	\
								TCCR1B &= ~(1<<CS12); 	\
								TCNT1 = 0x0000

#define Enable_InputCapture 	TIMSK |= (1<<TICIE1)
#define Disable_InputCapture 	TIMSK &= ~ (1<<TICIE1)

#define Enable_OutputCompare	TIMSK |= (1<<OCIE1A)
#define Disable_OutputCompare 	TIMSK &= ~ (1<<OCIE1A)

#define Enable_TxTimer 			TCNT0 = 0xFF - t_unit + 10;	\
								TIFR  =  (1<<TOV0);		\
								TIMSK |= (1<<TOIE0)

#define Disable_TxTimer 		TIMSK &= ~(1<<TOIE0)

#define RX_PIN (PIND & (1<<PD6))

#define ENABLE_SW_PRINTF()		SWuart_EnablePrintf()

//Rx Buffer size in bytes
#define RXBUFF_SIZE 20
#define TX_BUFF_SIZE 80

// new types
typedef enum SWuart_Status_E{
	SWuart_Rx_No_Data,
	SWuart_Rx_OK,
	SWuart_Rx_OK_Again,
	SWuart_Rx_OverFlow,
	SWuart_Tx_Ok,
	SWuart_Tx_BufferFull,
	SWuart_TX_Buffered,
	SWuart_Tx_OverFlow,
	SWuart_MEM_ERR
	}SWuart_Status_T;

typedef enum SWuart_state_E{
	SWuart_transmitting,
	SWuart_trans_in_byte,
	SWuart_trans_overflow,
	SWuart_idle
}SWuart_state_T;
// function prototypes
void SWuart_Init(uint16_t BaudRate);
SWuart_Status_T SWuart_Tx(uint8_t byte);
SWuart_Status_T SWuart_Rx(uint8_t *byte);
SWuart_Status_T SWuart_Tx_Str(char *byte);
void SWuart_EnablePrintf(void);

#endif /* SWuart_H_ */
