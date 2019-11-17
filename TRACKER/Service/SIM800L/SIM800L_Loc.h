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

typedef enum SIM800_Status_E{
	SIM800_TestAliv,
	SIM800_SetErrorMode,
	SIM800_NetRegister,
	SIM800_NetRegisterOk,
	SIM800_NetAtt,
	SIM800_SetAccessPoint,
	SIM800_SetGprsConn,
	SIM800_ReadIpAdd,
	SIM800_StartUDP,
	SIM800_SendCommand,
	SIM800_waitForData,
	SIM800_SendData,
	SIM800_CloseConn,
	SIM800_endFun,
	SIM800_RxWaiting,
	SIM800_IpWaiting
}SIM800_Status_t;
typedef enum sim800_state_e{
	sim800_init,
	sim800_searching_for_network,
	sim800_network_connected,
	sim800_connecting_to_internet,
	sim800_sending_data,
	sim800_ready
}sim800_state_t;
static struct SIM800L_S{
	int (*tx)(const char * ch,...);
	void (*rxWaitFor)(char * ch,uint8_t size,uint16_t timeout,SIM800_Status_t next_state);
	void (*readIpAddress)(uint16_t timeout,SIM800_Status_t next_state);
	sim800_state_t stat;
}sim800l;

struct UDP_S{
	uint8_t txtSize;
	char *txt;
}UDP;
static struct wait_S{
	SIM800_Status_t next_state,prev_state;
	const char *compare_string;
	uint16_t waitTime,currentTime; // waiting time in multiples of 5 milliseconds
	uint8_t string_size;

}wait;

static SIM800_Status_t SIM800_Status = SIM800_TestAliv;
static char IpAddress[] = "   .   .   .   ";
const char emptyIpAdd[] = "   .   .   .   ";
// Local state machine calls
void TestOK(void);
void SetErrorMode(void);
void NetRegister(void);
void NetRegisterOk(void);
void NetAtt(void);
void SetAccessPoint(void);
void SetGprsConn(void);
void ReadIpAdd(void);
void StartUDP(void);
void waitForData(void);
void SendCommand(void);
void SendData(void);
void CloseConn(void);
void endFun(void);
void rxWaitFor(char * ch,uint8_t size,uint16_t timeout,SIM800_Status_t next_state);
void readIpAddress(uint16_t timeout,SIM800_Status_t next_state);
void IpWaiting(void);
void waiting(void);
#endif /* SIM800L_LOC_H_ */
