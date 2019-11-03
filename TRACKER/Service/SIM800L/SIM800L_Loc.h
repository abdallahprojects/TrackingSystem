/*
 * SIM800L_Loc.h
 * Local include file, will be included only in SIM800L.c file
 * This file has all the local declarations
 * Created: 11/2/2019 1:15:10 PM
 *  Author: asere
 */ 


#ifndef SIM800L_LOC_H_
#define SIM800L_LOC_H_

#define COMMAND(x)	x,(uint8_t)sizeof(x)

typedef enum SIM800_St_E{
	SIM800_TestAliv,
	SIM800_NetAtt,
	SIM800_RxWaiting
}SIM800_St_t;

static struct SIM800L_S{
	int (*tx)(const char * ch,...);
	void (*rxWaitFor)(char * ch,uint8_t size,uint16_t timeout,SIM800_St_t next_state);
}sim800l;

static struct wait_S{
	SIM800_St_t next_state,prev_state;
	const char *compare_string;
	uint16_t waitTime,currentTime; // waiting time in multiples of 5 milliseconds
	uint8_t string_size;

}wait;

static SIM800_St_t SIM800_State = SIM800_TestAliv;

// Local state machine calls
void TestOK(void);
void NetAtt(void);
void rxWaitFor(char * ch,uint8_t size,uint16_t timeout,SIM800_St_t next_state);
void waiting(void);
#endif /* SIM800L_LOC_H_ */
