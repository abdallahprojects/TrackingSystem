/*
 * SWuart.h
 *
 * Created: 10/6/2019 1:07:37 PM
 *  Author: asere
 */ 


#ifndef SWuart_H_
#define SWuart_H_
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
	SWuart_Rx_No_Data,
	SWuart_Rx_OK,
	SWuart_Rx_OK_Again,
	SWuart_Rx_OverFlow,
	SWuart_Tx_Ok,
	SWuart_Tx_BufferFull,
	SWuart_TX_Buffered,
	SWuart_MEM_ERR
	}Status_T;

typedef enum SWuart_state_E{
	SWuart_transmitting,
	SWuart_trans_in_byte,
	SWuart_idle
}SWuart_state_T;
// function prototypes
void Init_SWuart(uint16_t BaudRate);
Status_T Tx_SWuart(uint8_t byte);
Status_T Rx_SWuart(uint8_t *byte);
Status_T Tx_SWuart_Str(char *byte);

#endif /* SWuart_H_ */
