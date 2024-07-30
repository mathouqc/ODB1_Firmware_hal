/*
 * L76LM33_tests.c
 *
 *  Created on: Jul 30, 2024
 *      Author: mathi
 */

#include "GAUL_Drivers/Tests/L76LM33_tests.h"

#include <stdio.h>
#include <string.h>

static L76LM33 L76_data;

extern uint8_t L76_NMEA_Buffer[];

void L76LM33_TESTS_ReadSentence_LogUART(UART_HandleTypeDef *huart) {
    // Debug timer High (to measure execution time with a digital analyzer)
    HAL_GPIO_WritePin(DEBUG_GPIO_Port, DEBUG_Pin, GPIO_PIN_SET);

	if (L76LM33_ReadSentence() == 0) {
		HAL_UART_Transmit(huart, L76_NMEA_Buffer, strnlen((char *)L76_NMEA_Buffer, 128), 100);
		HAL_UART_Transmit(huart, (uint8_t *)"\n", 1, 100);
	}

    // Debug timer Low (to measure execution time with a digital analyzer)
    HAL_GPIO_WritePin(DEBUG_GPIO_Port, DEBUG_Pin, GPIO_PIN_RESET);
}

void L76LM33_TESTS_ReadSentence_LogSTLINK() {
    // Debug timer High (to measure execution time with a digital analyzer)
    HAL_GPIO_WritePin(DEBUG_GPIO_Port, DEBUG_Pin, GPIO_PIN_SET);

	if (L76LM33_ReadSentence() == 0) {
		printf("%s\n", L76_NMEA_Buffer);
	}

    // Debug timer Low (to measure execution time with a digital analyzer)
    HAL_GPIO_WritePin(DEBUG_GPIO_Port, DEBUG_Pin, GPIO_PIN_RESET);
}

void L76LM33_TESTS_Read_LogSTLINK() {
    // Debug timer High (to measure execution time with a digital analyzer)
    HAL_GPIO_WritePin(DEBUG_GPIO_Port, DEBUG_Pin, GPIO_PIN_SET);

	if (L76LM33_Read(&L76_data) == 0) {
		printf("%s\n", L76_NMEA_Buffer);
	    L76LM33_TESTS_LogStructure(&L76_data);
	}

    // Debug timer Low (to measure execution time with a digital analyzer)
    HAL_GPIO_WritePin(DEBUG_GPIO_Port, DEBUG_Pin, GPIO_PIN_RESET);
}

void L76LM33_TESTS_LogStructure(L76LM33 *L76_data) {
	printf("Status:  %s\n", L76_data->status == 1 ? "OK" : "Error");
	printf("Fix:  %s\n", L76_data->fix == 1 ? "Yes" : "No");
	printf("Lat:  %f\n", L76_data->latitude);
	printf("Lon:  %f\n", L76_data->longitude);
}
