/*
 * SIM800L.h
 *
 * Created: 11/2/2019 12:05:56 PM
 *  Author: asere
 */ 


#ifndef SIM800L_H_
#define SIM800L_H_

// definition of exported vars and datatypes
typedef enum state_E{
	SIM800L_error,
	SIM800L_null_pointer,
	SIM800L_wrongSize,
	SIM800L_not_ready,
	SIM800L_send_ok
}state_t;


void SIM800L_Init(void);
void SIM800L_Cyclic5ms(void);

state_t SIM800L_SendUDP(char *ch,uint8_t size);

#endif /* SIM800L_H_ */
