/*
 * ublox_6m.c
 *
 *  Created on: Oct 13, 2019
 *      Author: asere
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "SWuart.h"
#include "ublox_6m.h"

static GPS_data_t GPS_data;

uint8_t scanCh(uint8_t * ch);
void ExTime(uint8_t ch);
void ExGPGGA3(uint8_t InputChar);
void ExLatitude(uint8_t ch);
void ExLatNS(uint8_t ch);
void ExLongitude(uint8_t ch);
void ExLongEW(uint8_t ch);
void ExSatFix(uint8_t ch);
void ExNumOfSats(uint8_t ch);


void UBLOX_Init(void)
{
	SWuart_Init(9600);
}

void UBLOX_cyclic(void)
{
	uint8_t ch;
	if(scanCh(&ch)){
		switch(GPS_data.ublox_state){
		case ublox_reading_GPGGA:
			ExGPGGA3(ch);
			break;
		case ublox_reading_time:
			ExTime(ch);
			break;
		case ublox_reading_latitude:
			ExLatitude(ch);
			break;
		case ublox_reading_latNS:
			ExLatNS(ch);
			break;
		case ublox_reading_longitude:
			ExLongitude(ch);
			break;
		case ublox_reading_longEW:
			ExLongEW(ch);
			break;
		case ublox_reading_SatFix:
			ExSatFix(ch);
			break;
		case ublox_reading_NumOfSats:
			ExNumOfSats(ch);
			// State is reset back to read GPGGA
			break;
		default:
			GPS_data.ublox_state = ublox_reading_GPGGA;
		}
	}

}
uint8_t scanCh(uint8_t * ch){
	SWuart_Status_T status;
		status = SWuart_Rx(ch);
		if(status == SWuart_Rx_OK || status == SWuart_Rx_OK_Again){
			return TRUE;
		}else{
			return FALSE;
		}
}
void ExTime(uint8_t ch){
	static uint8_t time_indx;
	//static uint8_t sec_buf;
	if(ch == ',')
	{
		GPS_data.ublox_state = ublox_reading_latitude;
		time_indx = 0;
	}else{
		time_indx++;
		if(time_indx < 3) // reading hours
		{
			if(time_indx == 1){
			//	if((ch-48)<=2)
				GPS_data.gps_hr = (ch-48)*10;
			}else{
			//	if((ch-48)<=9)
				GPS_data.gps_hr += (ch-48);
			}
		}else if(time_indx < 5) // reading hours
		{
			if(time_indx == 3){
			//	if((ch-48)<=5)
				GPS_data.gps_min = (ch-48)*10;
			}else{
			//	if((ch-48)<=9)
				GPS_data.gps_min += (ch-48);
			}
		}else if(time_indx < 7) // reading hours
		{
			if(time_indx == 5){
			//	if((ch-48)<=5)
				GPS_data.gps_sec = (ch-48)*10;
			}else{
			//	if((ch-48)<=9)
				GPS_data.gps_sec += (ch-48);
			}
		}else{
			// zero time pads ex: 183321(.00)
		}

	}
}
void ExLatitude(uint8_t ch){
	static uint8_t time_indx;
	if(ch == ',')
	{
		GPS_data.ublox_state = ublox_reading_latNS;
		time_indx = 0;
	}else{
		if(ch == '.'){
			// start reading seconds

		}
		time_indx++;
		if(time_indx < 3) // reading latitude degrees
		{
			if(time_indx == 1){
				GPS_data.lat_deg = (ch-48)*10;
			}else{
				GPS_data.lat_deg += (ch-48);
				if(GPS_data.lat_deg > 90) // if degrees are bigger than 90, then something went wrong!
					GPS_data.lat_deg = 0;
			}
		}else if(time_indx < 5) // reading latitude minutes
		{
			if(time_indx == 3){
				GPS_data.lat_min = (ch-48)*10;
			}else{
				GPS_data.lat_min += (ch-48);
			}
		}else if(time_indx < 11) // reading latitude seconds hours
		{
			if(time_indx == 6){
				GPS_data.lat_sec = (uint32_t)(ch-48)*10000;
			}else if(time_indx == 7){
				GPS_data.lat_sec += (uint32_t)(ch-48)*1000;
			}else if(time_indx == 8){
				GPS_data.lat_sec += (uint32_t)(ch-48)*100;
			}else if(time_indx == 9){
				GPS_data.lat_sec += (uint32_t)(ch-48)*10;
			}else{ //(time_indx == 10)
				GPS_data.lat_sec += (uint32_t)(ch-48);
			}
		}else{
			// unexpected extra chars
		}
	}

}
void ExLatNS(uint8_t ch){
	if(ch == ',')
		{
			GPS_data.ublox_state = ublox_reading_longitude;
		}else{
			GPS_data.Lat_NS = ch;
		}
}
void ExLongitude(uint8_t ch){
	static uint8_t time_indx;
	if(ch == ',')
	{
		GPS_data.ublox_state = ublox_reading_longEW;
		time_indx = 0;
	}else{
		if(ch == '.'){
			// start reading seconds

		}
		time_indx++;
		if(time_indx < 4) // reading latitude degrees
		{
			if(time_indx == 1){
				GPS_data.long_deg = (ch-48)*100;
			}else if(time_indx == 2){
				GPS_data.long_deg += (ch-48)*10;
			}else{
				GPS_data.long_deg += (ch-48);
				if(GPS_data.long_deg > 180) // if degrees are bigger than 90, then something went wrong!
					GPS_data.long_deg = 0;
			}
		}else if(time_indx < 6) // reading latitude minutes
		{
			if(time_indx == 4){
				GPS_data.long_min = (ch-48)*10;
			}else{
				GPS_data.long_min += (ch-48);
			}
		}else if(time_indx < 12) // reading latitude seconds hours
		{
			if(time_indx == 7){
				GPS_data.long_sec = (uint32_t)(ch-48)*10000;
			}else if(time_indx == 8){
				GPS_data.long_sec += (uint32_t)(ch-48)*1000;
			}else if(time_indx == 9){
				GPS_data.long_sec += (uint32_t)(ch-48)*100;
			}else if(time_indx == 10){
				GPS_data.long_sec += (uint32_t)(ch-48)*10;
			}else{ //(time_indx == 11)
				GPS_data.long_sec += (uint32_t)(ch-48);
			}
		}else{
			// unexpected extra chars
		}
	}

}
void ExLongEW(uint8_t ch){
	if(ch == ',')
		{
			GPS_data.ublox_state = ublox_reading_SatFix;
		}else{
			GPS_data.Long_EW = ch;
		}
}
void ExSatFix(uint8_t ch){
	if(ch == ',')
		{
			GPS_data.ublox_state = ublox_reading_NumOfSats;
		}else{
			if((ch-48) == 1)
			{
				// we've a fix (hot)
				GPS_data.gps_fix = 1;
			}else if((ch-48)==0 ){
				// Searching (warm)
			}else{
				// searching (warm)
				GPS_data.gps_fix = (ch-48);
			}
		}
}
void ExNumOfSats(uint8_t ch){
	// Num of Sats provided in two digits
	static uint8_t time_indx;
	if(ch == ',')
		{
			GPS_data.ublox_state = ublox_reading_GPGGA;
			time_indx = 0;
		}else{
			time_indx++;
			if(time_indx < 3) // reading latitude degrees
				{
					if(time_indx == 1){
						GPS_data.gps_numSats = (ch-48)*10;
					}else{
						GPS_data.gps_numSats += (ch-48);
					}
				}
		}
}

void ExGPGGA3(uint8_t InputChar)
{
	uint8_t ch[] ="$GPGGA,";
	static uint8_t i = 0;
	if(InputChar == ch[i])
	{
		i++;
		}else if(InputChar == ch[0]){
		i = 1;
		}else{
		i = 0;
	}
	if(i == sizeof(ch)-1)
	{
		GPS_data.ublox_state = ublox_reading_time;
		}else{
		GPS_data.ublox_state = ublox_reading_GPGGA;
	}
}
void UBLOX_readGPSData(GPS_data_t *readData){
	memcpy(readData,&GPS_data,sizeof(GPS_data_t));
}
