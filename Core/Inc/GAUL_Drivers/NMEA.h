/*
 * NMEA.h
 *
 *  Created on: May 12, 2024
 *      Author: gagnon
 *
 *  Edited on: Jul 4, 2024
 *      Autor: mathouqc
 */

#include "stm32f1xx_hal.h"

#ifndef INC_GAUL_DRIVERS_NMEA_H
#define INC_GAUL_DRIVERS_NMEA_H

#define NMEA_MAX_TOKEN_TO_READ 7
#define NMEA_MAX_RMC_LENGTH 90

typedef struct {
	uint8_t hours;		// Hours when GPS fix acquired
	uint8_t minutes;	// Minutes when GPS fix acquired
	float seconds;		// Seconds when GPS fix acquired
} time_t;

typedef struct {
    time_t time;		// Time when GPS fix acquired
    int8_t fix;			// 1: GPS Fix, 0: No GPS Fix
    float latitude;		// Latitude in Decimal Degrees
    float longitude;	// Longitude in Decimal Degrees
} GPS_Data;

int8_t NMEA_ValidateRMC(const char *nmea_sentence);
int8_t NMEA_ParseRMC(const char *nmea_sentence, GPS_Data *gps_data);

#endif /* INC_GAUL_DRIVERS_NMEA_H */
