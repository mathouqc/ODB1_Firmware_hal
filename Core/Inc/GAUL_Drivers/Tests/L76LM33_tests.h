/*
 * L76LM33_tests.h
 *
 *  Created on: Jul 30, 2024
 *      Author: mathi
 */

#include "stm32f1xx_hal.h"

#include "GAUL_Drivers/L76LM33.h"

#ifndef INC_GAUL_DRIVERS_TESTS_L76LM33_TESTS_H_
#define INC_GAUL_DRIVERS_TESTS_L76LM33_TESTS_H_

#define DEBUG_Pin GPIO_PIN_5
#define DEBUG_GPIO_Port GPIOB

void L76LM33_TESTS_ReadSentence_LogUART(UART_HandleTypeDef *huart);
void L76LM33_TESTS_ReadSentence_LogSTLINK();
void L76LM33_TESTS_Read_LogSTLINK();

void L76LM33_TESTS_LogStructure(L76LM33 *L76_data);

#endif /* INC_GAUL_DRIVERS_TESTS_L76LM33_TESTS_H_ */
