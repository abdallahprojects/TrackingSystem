/*
 * TRACKER.c
 *
 * Created: 10/6/2019 12:00:38 PM
 * Author : asere
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <SIM800L.h>
#include <SWuart.h>
#include <HWuart.h>
#include "LEDs.h"
#include <ublox_6m.h>
#include <OS.h>
void ubloxTask(void);
void LEDTask(void);
void PrintDiag(void);
void OS_Cyclic5ms(void);

int main(void)
{
	// flip PA2 status
    /* Replace with your application code */
	Init_OS();
	/* start ublox task to be invoked every 500ms as it should be less than
	 * 960 us, as we're currently working with 9600 Baudrate uart receiver
	*/
	UBLOX_Init();
	SIM800L_Init();
	OS_CreateNewTask(ubloxTask,5); // call ublox uart service every 500 us
	OS_CreateNewTask(LEDTask,100000l); // call led task every ten seconds
	OS_CreateNewTask(PrintDiag,10000); // call led task every one second
	OS_CreateNewTask(OS_Cyclic5ms,50);

    while (1) 
    {
    	OS_ServeOS();
		// endless counter
    }
}
void OS_Cyclic5ms(void)
{
	SIM800L_Cyclic5ms();
}
void ubloxTask(void){
	UBLOX_cyclic();
}
void LEDTask(void){
	//static char t[]="Hello\n";
	//SIM800L_SendUDP(t,(sizeof(t)/sizeof(t[0]))-1);
}
void PrintDiag(void){
	GPS_data_t readData;
	static char string[30];
	uint8_t size;
	LED_FLIP(LED1);
	UBLOX_readGPSData(&readData);
	ENABLE_SW_PRINTF();
	size = sprintf(string,"Time =  %dh,%dm,%ds \r\n",((readData.gps_hr+19)%24),readData.gps_min,readData.gps_sec);
	SIM800L_SendUDP(string,size);
	printf("Location = %u %u.%lu %c , %u %u.%lu %c \r\n",readData.lat_deg,readData.lat_min,readData.lat_sec,readData.Lat_NS,\
														readData.long_deg,readData.long_min,readData.long_sec,readData.Long_EW);
	printf("Number of Sats = %u \r\n\r\n",readData.gps_numSats);
}
