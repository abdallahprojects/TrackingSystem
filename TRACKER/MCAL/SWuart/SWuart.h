/*
 * SWuart.h
 *
 * Created: 10/6/2019 1:07:37 PM
 *  Author: asere
 */ 


#ifndef SWUART_H_
#define SWUART_H_


#define Stop_Timer1 	TCCR1B &= ~(1<<CS11); \
						TCCR1B &= ~(1<<CS10); \
						TCCR1B &= ~(1<<CS12)

#define Start_Timer1 	TCCR1B |= (1<<CS11); \
						TCCR1B &= ~(1<<CS10); \
						TCCR1B &= ~(1<<CS12); \
						TCNT1 = 0x0000

#define Enable_InputCapture TIMSK |= (1<<TICIE1)
#define Disable_InputCapture TIMSK &= ~ (1<<TICIE1)

#define Enable_OutputCompare TIMSK |= (1<<OCIE1A)
#define Disable_OutputCompare TIMSK &= ~ (1<<OCIE1A)

#define RX_PIN (PIND & (1<<PD6))

//Rx Buffer size in bytes
#define BUFF_SIZE 100

void INIT_uart(uint16_t BaudRate);

typedef enum Status_E{
	UART_No_Data,
	UART_OK,
	UART_OK_Again,
	UART_OverFlow
	}Status_T;

#endif /* SWUART_H_ */