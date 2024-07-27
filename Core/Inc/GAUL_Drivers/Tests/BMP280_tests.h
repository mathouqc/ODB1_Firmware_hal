/*
 * BMP280_tests.h
 *
 *  Created on: Jul 8, 2024
 *      Author: mathouqc
 */

#include "stm32f1xx_hal.h"

#ifndef INC_GAUL_DRIVERS_TESTS_BMP280_TESTS_H_
#define INC_GAUL_DRIVERS_TESTS_BMP280_TESTS_H_

#define DEBUG_Pin GPIO_PIN_5
#define DEBUG_GPIO_Port GPIOB

int8_t BMP280_TESTS_LogUART(UART_HandleTypeDef *huart);
int8_t BMP280_TESTS_LogSTLINK();

#endif /* INC_GAUL_DRIVERS_TESTS_BMP280_TESTS_H_ */
