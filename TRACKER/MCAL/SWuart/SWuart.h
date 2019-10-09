/*
 * SWuart.h
 *
 * Created: 10/6/2019 1:07:37 PM
 *  Author: asere
 */ 


#ifndef SWUART_H_
#define SWUART_H_
// Configuration settings
#define TXDDR  DDRD
#define TXPORT PORTD
#define TXPIN PD7
#define STOP_BYTES 1

///
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

//Rx Buffer size in bytes
#define RxBUFF_SIZE 100
#define TX_BUFF_SIZE 50

// new types
typedef enum Status_E{
	UART_Rx_No_Data,
	UART_Rx_OK,
	UART_Rx_OK_Again,
	UART_Rx_OverFlow,
	UART_Tx_Ok,
	UART_Tx_BufferFull,
	UART_TX_Buffered,
	UART_MEM_ERR
	}Status_T;

typedef enum uart_state_E{
	uart_transmitting,
	uart_trans_in_byte,
	uart_idle
}uart_state_T;
// function prototypes
void INIT_uart(uint16_t BaudRate);
Status_T Tx_UART(uint8_t byte);
Status_T Rx_UART(uint8_t *byte);
Status_T Tx_UART_Str(char *byte);

#endif /* SWUART_H_ */
