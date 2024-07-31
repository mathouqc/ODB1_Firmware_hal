/*
 * BMP280.h
 *
 *  Created on: May 18, 2024
 *      Author: gagnon
 *
 *  Edited on: Jul 4, 2024
 *  	Autor: mathouqc
 */

#include "stm32f1xx_hal.h"

#ifndef INC_GAUL_DRIVERS_BMP280_H_
#define INC_GAUL_DRIVERS_BMP280_H_

#define BMP_CS_Pin              GPIO_PIN_8
#define BMP_CS_GPIO_Port        GPIOA

#define BMP280_SPI_TIMEOUT      100

#define BMP280_DEVICE_ID        0x58
#define BMP280_RESET_VALUE      0xB6

#define BMP280_REG_TEMP_XLSB    0xFC
#define BMP280_REG_TEMP_LSB     0xFB
#define BMP280_REG_TEMP_MSB     0xFA
#define BMP280_REG_PRESS_XLSB   0xF9
#define BMP280_REG_PRESS_LSB    0xF8
#define BMP280_REG_PRESS_MSB    0xF7
#define BMP280_REG_CONFIG       0xF5
#define BMP280_REG_CTRL_MEAS    0xF4
#define BMP280_REG_STATUS       0xF3
#define BMP280_REG_RESET        0xE0
#define BMP280_REG_ID           0xD0
#define BMP280_REG_CALIB_00     0x88

#define BMP280_MODE_LOW_POWER 0
#define BMP280_MODE_NORMAL_POWER 1

// Setting ctrl_meas (data acquisition) register (temp/press oversampling and power mode)
// Temperature (osrs_t)    osrs_t x2    010
// Pressure (osrs_p)       osrs_p x8    100
// Power mode              normal       11  (sleep 00 ; force 01/10 ; normal 11)
// 010;100;11 = 0x53 (normal)
#define BMP280_SETTING_CTRL_MEAS_LOW 0x53

// Setting ctrl_meas (data acquisition) register (temp/press oversampling and power mode)
// Temperature (osrs_t)    osrs_t x2    010
// Pressure (osrs_p)       osrs_p x16   101
// Power mode              normal       11  (sleep 00 ; force 01/10 ; normal 11)
// 010;101;11 = 0x57 (normal)
#define BMP280_SETTING_CTRL_MEAS_NORMAL 0x57

//Setting config register (rate, filter, interface options)
//Stanby time    62.5ms  001
//IIR filter     16x     100
//Bit 1          N/A     0
//Spi3w          4wire   0
//001;100;00 = 0x30
#define BMP280_SETTING_CONFIG_LOW 0x30

//Setting config register (rate, filter, interface options)
//Stanby time    0.5ms   000
//IIR filter     16x     100
//Bit 1          N/A     0
//Spi3w          4wire   0
//000;100;00 = 0x10
#define BMP280_SETTING_CONFIG_NORMAL 0x10


typedef struct {
    uint16_t 	dig_T1;
    int16_t 	dig_T2;
    int16_t 	dig_T3;
    uint16_t 	dig_P1;
    int16_t 	dig_P2;
    int16_t 	dig_P3;
    int16_t 	dig_P4;
    int16_t 	dig_P5;
    int16_t 	dig_P6;
    int16_t 	dig_P7;
    int16_t 	dig_P8;
    int16_t 	dig_P9;
} BMP280_CalibData;

typedef struct {
    float 				press_Pa;
    float				press_ref_Pa;
    float 				temp_C;
    float				alt_m;
    int32_t 			t_fine;
    BMP280_CalibData 	calib_data;
} BMP280;

int8_t BMP280_Init(BMP280 *BMP_data, SPI_HandleTypeDef *hspi);

int8_t BMP280_SetMode(uint8_t mode);
int8_t BMP280_ReadCalibrationData(BMP280 *BMP_data);
int8_t BMP280_MeasureReference(BMP280 *BMP_data, uint16_t samples, uint8_t delay);

int8_t BMP280_ReadTemperature(BMP280 *BMP_data);
int8_t BMP280_ReadPressure(BMP280 *BMP_data);

int8_t BMP280_ReadAltitude(BMP280 *BMP_data);
float BMP280_PressureToAltitude(float pressure, float pressure_ref);

int8_t BMP280_SoftReset();
int8_t BMP280_Read(uint8_t reg, uint8_t RX_Buffer[], uint8_t size);
int8_t BMP280_Write(uint8_t reg, uint8_t value);

#endif /* INC_GAUL_DRIVERS_BMP280_H_ */
