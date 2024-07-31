/*
 * L76LM33.h
 *
 *  Created on: May 12, 2024
 *      Author: gagnon
 *
 *  Edited on: Jul 4, 2024
 *      Autor: mathouqc
 */

#include "stm32f1xx_hal.h"

#ifndef INC_GAUL_DRIVERS_L76LM33_H_
#define INC_GAUL_DRIVERS_L76LM33_H_

#define L76LM33_BUFFER_SIZES 256  // NMEA sentence is around 80 char max, has to be a power of two.
#define L76LM33_UART_TIMEOUT 1000 // For UART transmit

typedef struct {
	uint8_t status; // 1: OK, 0: Error
	uint8_t fix; // 1: GPS Fix, 0: No GPS Fix
	float latitude;
	float longitude;
} L76LM33;

int8_t L76LM33_Init(UART_HandleTypeDef *huart);

void L76LM33_RxCallback(UART_HandleTypeDef *huart);

int8_t L76LM33_Read(L76LM33 *L76_data);
int8_t L76LM33_ReadSentence();

int8_t L76LM33_SendCommand(char command[], uint8_t size);

#endif /* INC_GAUL_DRIVERS_L76LM33_H_ */
