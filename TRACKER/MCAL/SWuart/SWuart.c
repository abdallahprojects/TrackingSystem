/*
 * SWuart.c
 *
 * Created: 10/6/2019 1:07:25 PM
 *  Author: asere
 */ 
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "SWuart.h"
// clocks per one bit
static uint16_t t_unit;
static volatile uint16_t InputCapture;
static volatile uint8_t RxBuffer[RxBUFF_SIZE+1];
static volatile uint8_t TxBuffer[TX_BUFF_SIZE+1];
static volatile uint8_t TxBufferPointer,TxByte,RxBufferPointer;
static volatile uint8_t bit = 10;
static volatile SWuart_state_T Txstate= SWuart_idle;
static volatile SWuart_state_T Rxstate= SWuart_idle;
static volatile Status_T RxStatus = SWuart_Rx_No_Data;
void Init_SWuart(uint16_t BaudRate)
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

	
	// Enable Global Interrupt
	sei();
	
}

ISR(TIMER1_CAPT_vect){
	cli(); // disable global Interrupt
	DDRA = 0xFF;
	PORTA = 0x04;
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
		RxBuffer[RxBufferPointer] = byte;
		RxBufferPointer++;
		if(RxBufferPointer >= RxBUFF_SIZE)
		{
			RxStatus = SWuart_Rx_OverFlow;
			RxBufferPointer = 0;
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
			TxByte = TxBuffer[--TxBufferPointer]; //pop
			Txstate = SWuart_trans_in_byte;
			// Next line for debugging only
			// TxBuffer[TxBufferPointer]=0;
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
				if(TxBufferPointer == 0){
					Disable_TxTimer;
					Txstate = SWuart_idle;
				}
	}else{
			//something went wrong
			bit = 0;
		}
}
Status_T Rx_SWuart(uint8_t * byte)
{
	Status_T ret;
	uint8_t i;
	if(RxStatus != SWuart_Rx_OverFlow)
	{
		 if (RxBufferPointer == 0){
				ret =  SWuart_Rx_No_Data;
		}else
		{
			cli();
			*byte = RxBuffer[0];
			RxBufferPointer--;
			for(i=0;i<RxBufferPointer;i++){
				RxBuffer[i] = RxBuffer[i+1];
			}

			sei();
			if (RxBufferPointer == 0){
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
Status_T Tx_SWuart(uint8_t byte)
{
	Status_T ret;
	uint8_t i,tmp;
	// Don't update the buffer in the middle of byte transmission
	while(Txstate == SWuart_trans_in_byte);
	switch(Txstate){
	case SWuart_idle:
		// idle Txstate
		Enable_TxTimer;
		TxBuffer[TxBufferPointer++] = byte;
		Txstate = SWuart_transmitting;
		ret = SWuart_Tx_Ok;
		break;
	case SWuart_transmitting:
		// Add character to the queue buffer
		if(TxBufferPointer < TX_BUFF_SIZE)
		{
			cli(); // critical not to read while writing
			for(i=0;i<TxBufferPointer;i++)
			{
				tmp = TxBuffer[0];
				TxBuffer[0] = TxBuffer[i+1];
				TxBuffer[i+1] = tmp;
			}
			TxBuffer[0] = byte;
			TxBufferPointer++;
			sei();
			ret = SWuart_TX_Buffered;
		}else{
			ret = SWuart_Tx_BufferFull;
		}
		break;
	default:
		//exception
		ret = SWuart_MEM_ERR;
		break;
	}
	return ret;
}
Status_T Tx_SWuart_Str(char *byte)
{
	uint8_t i = 0;
	Status_T ret;
	while(byte[i]!= '\0')
	{
		ret = Tx_SWuart(byte[i]);
		i++;
	}
	ret = Tx_SWuart('\r');
	ret = Tx_SWuart('\n');
	return ret;
}
