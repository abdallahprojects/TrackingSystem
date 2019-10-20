/*
 * ublox_6m.h
 *
 *  Created on: Oct 13, 2019
 *      Author: asere
 */

#ifndef SERVICES_UBLOX_6M_UBLOX_6M_H_
#define SERVICES_UBLOX_6M_UBLOX_6M_H_

#define TRUE 1
#define FALSE 0
typedef enum ublox_state_E{
	ublox_reading_GPGGA,
	ublox_reading_time,
	ublox_reading_latitude,
	ublox_reading_latNS,
	ublox_reading_longitude,
	ublox_reading_longEW,
	ublox_reading_SatFix,
	ublox_reading_NumOfSats,
	ublox_reading_Accuracy,
	ublox_reading_MetersAboveSea
}ublox_state_T;

typedef struct GPS_data_S{
	ublox_state_T ublox_state;
	uint8_t gps_hr,gps_min,gps_sec;
	uint8_t lat_deg,lat_min,long_deg,long_min;
	uint32_t lat_sec, long_sec;
	char Lat_NS,Long_EW;
	uint8_t gps_fix;
	uint8_t gps_numSats;
	}GPS_data_t;

void ublox_cyclic(void);
void UBLOX_readGPSData(GPS_data_t *readData);
#endif /* SERVICES_UBLOX_6M_UBLOX_6M_H_ */
