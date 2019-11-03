/*
 * SWuart.c
 *
 * Created: 10/6/2019 1:07:25 PM
 *  Author: asere
 */ 
#include <stdint.h>

#include <stdio.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include "SWuart.h"
// clocks per one bit
static uint16_t t_unit;
static volatile uint16_t InputCapture;
static volatile uint8_t RxBuffer[RXBUFF_SIZE+1];
static volatile uint8_t TxBuffer[TX_BUFF_SIZE+1];
static volatile uint8_t TxBufferPointer,TxByte;
static volatile uint16_t RxBufferHead,RxBufferTail;
static volatile uint16_t TxBufferHead,TxBufferTail;
static volatile uint8_t bit = 10;
static volatile SWuart_state_T Txstate= SWuart_idle;
static volatile SWuart_state_T Rxstate= SWuart_idle;
static volatile SWuart_Status_T RxStatus = SWuart_Rx_No_Data;


int SWuartWrap_Tx(char ch,FILE * stream);
static FILE uart_str = FDEV_SETUP_STREAM(SWuartWrap_Tx, NULL, _FDEV_SETUP_RW);

void SWuart_Init(uint16_t BaudRate)
{
	
  /* divided by 8 because of the prescalar
   will have 156.25 in case of 9600 Baudrate is selected */
  	t_unit = F_CPU/BaudRate/8; 
	// init timer1 registers
	TCCR1A = 0x00;
	// use CTC mode of timer1, TOP = OCR1A, update = Immediate
	TCCR1B |= (1<<WGM12);
	TCCR1B &= ~(1<<WGM13);
	// Set the prescalar to 8
	TCCR1B |= (1<<CS11);
	TCCR1B &= ~(1<<CS10);
	TCCR1B &= ~(1<<CS12);
	// init Rx pin on ICP1 PD6
	DDRD &= ~(1<<PD6);
	PORTD |= (1<<PD6);  /// not sure assuming it's weak high pull up
	// write ACIC (Analog Comparator Input Capture) pin to zero to use ICP1 pin
	ACSR &= ~(1<<ACIC);
	// write ICNC for noise canceling to one to use the noise canceling
	TCCR1B |= (1<<ICNC1);
	// write ICES to detect on falling Edge
	TCCR1B &= ~(1<<ICES1);

	/////////////////////////////// Enable Timer 0 down-counting for Tx /////////////
	TXDDR |= (1<<TXPIN); // Set Tx pin as output
	TXPORT |= (1<<TXPIN); // set Tx pin high for no data transmission
	// set timer for counting up and interrupt when reaches 0xFF
	TCCR0 &= ~(1<<WGM00);
	TCCR0 &= ~(1<<WGM01);
	TCCR0 &= ~(1<<COM00);
	TCCR0 &= ~(1<<COM01);
	// set clkI/O/8 (From prescaler)
	TCCR0 |= (1<<CS01);


	/////////// End Timer 0 configuration ////////////////////////////////
	// Enable Input Capture Interrupt on Timer 1
	Enable_InputCapture;

}

void SWuart_EnablePrintf(void)
{
	stdout = &uart_str;
}

ISR(TIMER1_CAPT_vect){
	cli(); // disable global Interrupt
	// first thing first: read the ICP register
	InputCapture = ICR1; 
	// Start the timer compare value to read first bit
	OCR1A = (t_unit * 3)/2;
	TCNT1 = RX_TIMER_COMP;
	TIFR  = (1<<OCF1A);
	// Disable the Timer1 cap interrupt
	Disable_InputCapture;
	Enable_OutputCompare;
	sei(); //enable global interrupt
}

ISR(TIMER1_COMPA_vect){
	static uint8_t bitCounter;
	static uint8_t byte;
	cli();
	//configure out compare on one byte time
	OCR1A = (t_unit);
	TCNT1 = RX_TIMER_COMP;
	TIFR  = (1<<OCF1A);
	// read the data bits and increment bit counter
	if(bitCounter < 8)
	{
		
		byte = (RX_PIN) ? (byte|(1<<bitCounter)):(byte&~(1<<bitCounter));
		bitCounter++;
	}else{
		bitCounter = 0;
		RxBuffer[RxBufferHead] = byte;
		RxBufferHead = (RxBufferHead == RXBUFF_SIZE)? 0:(RxBufferHead+1);
		if((RxBufferHead+1 == RxBufferTail)||((RxBufferHead == RXBUFF_SIZE) && (RxBufferTail == 0)))
		{
			RxStatus = SWuart_Rx_OverFlow;
		}
		TIFR = (1<<ICF1);
		Disable_OutputCompare;
		Enable_InputCapture;
	}
	sei();

	// if data counter is less than 9 set the output compare to fire after 1 t_unit
	// else Enable the Timer1 cap interrupt and process byte in buffer
}
ISR(TIMER0_OVF_vect){
	//compensate for the interrupt delay 0x03
	TCNT0 = 0xFF - t_unit + 3 ;

	if(bit == 0){
			TXPORT &= ~(1 << TXPIN); // START BIT
			bit++;
			// Next line for debugging only
			// TxBuffer[TxBufferPointer]=0;
			// Circular buffer
			TxByte = TxBuffer[TxBufferTail++];
			Txstate = SWuart_trans_in_byte;
			if(TxBufferTail == TX_BUFF_SIZE+1)
				TxBufferTail = 0;

	} else if(bit < 9){
			//DATA BITS
			if((TxByte & 0x1)){ //0b10000000, test left most bit // DATA BITS
				TXPORT |= (1 << TXPIN);  // TXPEN HIGH
			} else {
				TXPORT &= ~(1 << TXPIN); // TXPEN LOW
			}
			TxByte >>= 1;
			bit++;
	} else if(bit == 9){
			//STOP BIT
			bit = 0;
			TXPORT |= (1 << TXPIN);  // STOP BIT
			Txstate = SWuart_transmitting;
			// Check Circular buffer for more data to read
			if (TxBufferTail == TxBufferHead){
				Disable_TxTimer;
				Txstate = SWuart_idle;
			}
	}else{
			//something went wrong
			bit = 0;
		}
}
SWuart_Status_T SWuart_Rx(uint8_t * byte)
{
	SWuart_Status_T ret;
	if(RxStatus != SWuart_Rx_OverFlow)
	{
		 if (RxBufferTail == RxBufferHead){
				ret =  SWuart_Rx_No_Data;
		}else
		{
			cli();
			*byte = RxBuffer[RxBufferTail++];
			if(RxBufferTail == RXBUFF_SIZE+1)
				RxBufferTail = 0;

			sei();
			if (RxBufferHead == RxBufferTail){
						ret =  SWuart_Rx_OK;
			}else{
				ret = SWuart_Rx_OK_Again;
			}
		}

	}else{
		ret = SWuart_Rx_OverFlow;
	}
	return ret;
}
SWuart_Status_T SWuart_Tx(uint8_t byte)
{
	SWuart_Status_T ret;

	// Don't update the buffer in the middle of byte transmission
	//while(Txstate == SWuart_trans_in_byte);
		// idle Txstate

		if((TxBufferHead+1 == TxBufferTail)||((TxBufferHead == TX_BUFF_SIZE) && (TxBufferTail == 0)))
			{
				ret = SWuart_Tx_OverFlow;
				//Txstate = SWuart_trans_overflow;
			}else{
				if(Txstate == SWuart_idle)
					Enable_TxTimer;
				//Txstate = SWuart_transmitting;
				TxBuffer[TxBufferHead] = byte;
				TxBufferHead = (TxBufferHead == TX_BUFF_SIZE)? 0:(TxBufferHead+1);
				ret = SWuart_Tx_Ok;
			}
	return ret;
}

int SWuartWrap_Tx(char ch,FILE * stream)
{
	SWuart_Tx(ch);
	return 0;
}


SWuart_Status_T SWuart_Str_Tx(char *byte)
{
	uint8_t i = 0;
	SWuart_Status_T ret;
	while(byte[i]!= '\0')
	{
		ret = SWuart_Tx(byte[i]);
		i++;
	}
	ret = SWuart_Tx('\r');
	ret = SWuart_Tx('\n');
	return ret;
}
