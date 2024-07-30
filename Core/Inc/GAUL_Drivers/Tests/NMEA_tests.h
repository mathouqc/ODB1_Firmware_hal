/*
 * NMEA_tests.h
 *
 *  Created on: Jul 25, 2024
 *      Author: mathouqc
 */

#include "stm32f1xx_hal.h"

#include "GAUL_Drivers/NMEA.h"

#ifndef INC_GAUL_DRIVERS_TESTS_NMEA_TESTS_H_
#define INC_GAUL_DRIVERS_TESTS_NMEA_TESTS_H_

#define DEBUG_Pin GPIO_PIN_5
#define DEBUG_GPIO_Port GPIOB

void NMEA_TESTS_ValidateRMC_LogUART(UART_HandleTypeDef *huart);
void NMEA_TESTS_ValidateRMC_LogSTLINK();

void NMEA_TESTS_ParseRMC_LogSTLINK();

void NMEA_TESTS_LogStructure(GPS_Data *gps_data);

#endif /* INC_GAUL_DRIVERS_TESTS_NMEA_TESTS_H_ */
