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
volatile static uint16_t InputCapture;
volatile static uint8_t RxBuffer[BUFF_SIZE+1];
void INIT_uart(uint16_t BaudRate)
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
	TCNT1 = 0x0003;
	TIFR  = (1<<OCF1A);
	// Disable the Timer1 cap interrupt
	Disable_InputCapture;
	Enable_OutputCompare;
	sei(); //enable global interrupt
}

ISR(TIMER1_COMPA_vect){
	static uint8_t bitCounter,NumInBytes;
	static uint8_t byte;
	//configure out compare on one byte time
	OCR1A = (t_unit);
	TCNT1 = 0x0003;
	TIFR  = (1<<OCF1A);
	// read the data bits and increment bit counter
	if(bitCounter < 8)
	{
		
		byte = (RX_PIN) ? (byte|(1<<bitCounter)):(byte&~(1<<bitCounter));
		bitCounter++;
	}else{
		bitCounter = 0;
		RxBuffer[NumInBytes] = byte;
		NumInBytes++;
		if(NumInBytes >= BUFF_SIZE)
		{
			NumInBytes = 0;
		}
		TIFR = (1<<ICF1);
		Disable_OutputCompare;
		Enable_InputCapture;
	}


	// if data counter is less than 9 set the output compare to fire after 1 t_unit
	// else Enable the Timer1 cap interrupt and process byte in buffer
}
Status_T Rx_UART(uint8_t * byte)
{
	
}