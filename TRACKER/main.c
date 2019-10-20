/*
 * TRACKER.c
 *
 * Created: 10/6/2019 12:00:38 PM
 * Author : asere
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <SWuart.h>

#include <ublox_6m.h>
#include <OS.h>
void ubloxTask(void);
void LEDTask(void);
void PrintDiag(void);
int main(void)
{
	// flip PA2 status
    /* Replace with your application code */
	DDRA |= 0xFF;
	Init_SWuart(9600);
	Init_OS();
	/* start ublox task to be invoked every 500ms as it should be less than
	 * 960 us, as we're currently working with 9600 Baudrate uart receiver
	*/

	OS_CreateNewTask(ubloxTask,5); // call ublox uart service every 500 us
	OS_CreateNewTask(LEDTask,10000); // call led task every one second
	OS_CreateNewTask(PrintDiag,10000); // call led task every one second

    while (1) 
    {
    	OS_ServeOS();
		// endless counter
    }
}
void ubloxTask(void){
	ublox_cyclic();
}
void LEDTask(void){
	PORTA ^= (1<<PA2);
}
void PrintDiag(void){
	GPS_data_t readData;
	UBLOX_readGPSData(&readData);
	printf("Time =  %dh,%dm,%ds \r\n",readData.gps_hr-4,readData.gps_min,readData.gps_sec);
	printf("Location = %u %u.%lu %c , %u %u.%lu %c \r\n",readData.lat_deg,readData.lat_min,readData.lat_sec,readData.Lat_NS,\
														readData.long_deg,readData.long_min,readData.long_sec,readData.Long_EW);
}
