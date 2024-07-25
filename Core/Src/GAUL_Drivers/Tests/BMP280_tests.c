/*
 * BMP280_tests.c
 *
 *  Created on: Jul 8, 2024
 *      Author: mathouqc
 */

#include "GAUL_Drivers/Tests/BMP280_tests.h"

#include "GAUL_Drivers/BMP280.h"
#include "stdio.h"

extern BMP280 bmp_data;

int8_t BMP280_TESTS_LogUART(UART_HandleTypeDef *huart) {
    // Debug timer High (to measure execution time with a digital analyzer)
    HAL_GPIO_WritePin(DEBUG_GPIO_Port, DEBUG_Pin, GPIO_PIN_SET);

    if (BMP280_ReadAltitude(&bmp_data) != 0) {
    	printf("Error BMP280_ReadAltitude\r\n");
    	return -1; // Error
    }

    // Debug timer Low (to measure execution time with a digital analyzer)
    HAL_GPIO_WritePin(DEBUG_GPIO_Port, DEBUG_Pin, GPIO_PIN_RESET);

    // UART log
    char Data[36];
    sprintf(Data, "%9.4f kPa %6.2f C %8.2f m\r\n", bmp_data.press_Pa / 1000, bmp_data.temp_C, bmp_data.alt_m);
    HAL_UART_Transmit(huart, (uint8_t *)Data, 36, 1000);

    return 0; // OK
}

int8_t BMP280_TESTS_LogSTLINK() {
    // Debug timer High (to measure execution time with a digital analyzer)
    HAL_GPIO_WritePin(DEBUG_GPIO_Port, DEBUG_Pin, GPIO_PIN_SET);

    if (BMP280_ReadAltitude(&bmp_data) != 0) {
    	printf("Error BMP280_ReadAltitude\r\n");
    	return -1; // Error
    }

    // Debug timer Low (to measure execution time with a digital analyzer)
    HAL_GPIO_WritePin(DEBUG_GPIO_Port, DEBUG_Pin, GPIO_PIN_RESET);

    // STLINK log
	printf("%9.4f kPa %6.2f C %8.2f m\r\n", bmp_data.press_Pa / 1000, bmp_data.temp_C, bmp_data.alt_m);

    return 0; // OK
}
