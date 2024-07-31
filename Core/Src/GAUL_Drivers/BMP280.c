/*
 * BMP280.h
 *
 * BMP280 is a barometer used to get the altitude of the rocket.
 * Main functions are "BMP280_Init" to configure BMP280, and "BMP280_ReadAltitude"
 * to update values in a BMP280 structure.
 *
 *  Created on: May 18, 2024
 *      Author: gagnon
 *
 *  Edited on: Jul 4, 2024
 *      Autor: mathouqc
 *
 */

#include "GAUL_Drivers/BMP280.h"

#include "math.h" // for pow()

// Pointer to SPI handler
SPI_HandleTypeDef *BMP_hspi;

// Multi purpose receiving buffer
uint8_t BMP_RX_Buffer[26];

/**
 * Initialize BMP280 sensor.
 * - Set BMP280 SPI handler
 * - Reset
 * - Validate SPI2 communication with device ID
 * - Read calibration data
 * - Set BMP280 configuration
 *
 * @param BMP_data: pointer to a BMP280 structure.
 * @param hspi: pointer to a HAL SPI handler.
 *
 * @retval 0 OK
 * @retval -1 ERROR
 */
int8_t BMP280_Init(BMP280 *BMP_data, SPI_HandleTypeDef *hspi) {
	// SPI Bug Fix
	/* Note page 704/1136 RM0008 Rev 21 :
	* The idle state of SCK must correspond to the polarity selected in the
	* SPI_CR1 register (by pulling up SCK if CPOL=1 or pulling down SCK if CPOL=0).
	*/
	uint8_t dummy = 0x00;
	HAL_SPI_Transmit(hspi, &dummy, 1, 1000);

	// Set BMP280 SPI handler
	BMP_hspi = hspi;

    // Reset
    if (BMP280_SoftReset() != 0) {
    	return -1; // SPI Error
    }

    // Check ID
    if (BMP280_Read(BMP280_REG_ID, BMP_RX_Buffer, 1) != 0) {
    	return -1; // SPI Error
    }
    if (BMP_RX_Buffer[0] != BMP280_DEVICE_ID) {
        return -1; // Error can't communicate or device not found
    }

    // Set data acquisition and configuration
    if (BMP280_SetMode(BMP280_MODE_NORMAL_POWER) != 0) {
    	return -1; // SPI Error
    }

    // Calibration
    if (BMP280_ReadCalibrationData(BMP_data) != 0) {
    	return -1; // SPI Error
    }

    // Delay to get first measurement
    HAL_Delay(200);

    // Set reference
    if (BMP280_MeasureReference(BMP_data, 40, 50) != 0 || BMP_data->press_ref_Pa < 90000.0 || BMP_data->press_ref_Pa > 110000.0) {
    	BMP_data->press_ref_Pa = 101325.0; // Default value if error or extreme values
    }

    return 0; // OK
}

/**
 * Choose BMP280 custom configuration.
 *
 * @param mode: BMP280_MODE_LOW_POWER or BMP280_MODE_NORMAL_POWER
 *
 * @retval 0 OK
 * @retval -1 SPI ERROR
 */
int8_t BMP280_SetMode(uint8_t mode) {
	if (BMP280_SoftReset() != 0) {
		return -1; // SPI ERROR
	}

    if (mode == BMP280_MODE_LOW_POWER) {
        // Set data acquisition options (temp/press oversampling and power mode)
    	if (BMP280_Write(BMP280_REG_CTRL_MEAS, BMP280_SETTING_CTRL_MEAS_LOW) != 0) {
    		return -1; // SPI ERROR
    	}
        // Set configuration (rate, filter and interface options)
        if (BMP280_Write(BMP280_REG_CONFIG, BMP280_SETTING_CONFIG_LOW) != 0) {
        	return -1; // SPI ERROR
        }
    } else {
        // Set data acquisition options (temp/press oversampling and power mode)
    	if (BMP280_Write(BMP280_REG_CTRL_MEAS, BMP280_SETTING_CTRL_MEAS_NORMAL) != 0) {
    		return -1; // SPI ERROR
    	}
        // Set configuration (rate, filter and interface options)
    	if (BMP280_Write(BMP280_REG_CONFIG, BMP280_SETTING_CONFIG_NORMAL) != 0) {
    		return -1; // SPI ERROR
    	}
    }

    return 0; //OK
}

/**
 * Reads calibration registers, then store them in the BMP280_CalibData structure.
 *
 * @param BMP_data: pointer to a BMP280 structure.
 *
 * @retval 0 OK
 * @retval -1 SPI ERROR
 */
int8_t BMP280_ReadCalibrationData(BMP280 *BMP_data) {
    if (BMP280_Read(BMP280_REG_CALIB_00, BMP_RX_Buffer, 26) != 0) {
    	return -1; // SPI ERROR
    }

    BMP_data->calib_data.dig_T1 = (BMP_RX_Buffer[1] << 8) | BMP_RX_Buffer[0];
    BMP_data->calib_data.dig_T2 = (BMP_RX_Buffer[3] << 8) | BMP_RX_Buffer[2];
    BMP_data->calib_data.dig_T3 = (BMP_RX_Buffer[5] << 8) | BMP_RX_Buffer[4];
    BMP_data->calib_data.dig_P1 = (BMP_RX_Buffer[7] << 8) | BMP_RX_Buffer[6];
    BMP_data->calib_data.dig_P2 = (BMP_RX_Buffer[9] << 8) | BMP_RX_Buffer[8];
    BMP_data->calib_data.dig_P3 = (BMP_RX_Buffer[11] << 8) | BMP_RX_Buffer[10];
    BMP_data->calib_data.dig_P4 = (BMP_RX_Buffer[13] << 8) | BMP_RX_Buffer[12];
    BMP_data->calib_data.dig_P5 = (BMP_RX_Buffer[15] << 8) | BMP_RX_Buffer[14];
    BMP_data->calib_data.dig_P6 = (BMP_RX_Buffer[17] << 8) | BMP_RX_Buffer[16];
    BMP_data->calib_data.dig_P7 = (BMP_RX_Buffer[19] << 8) | BMP_RX_Buffer[18];
    BMP_data->calib_data.dig_P8 = (BMP_RX_Buffer[21] << 8) | BMP_RX_Buffer[20];
    BMP_data->calib_data.dig_P9 = (BMP_RX_Buffer[23] << 8) | BMP_RX_Buffer[22];

    return 0; // OK
}

/**
 * Set reference temperature and pressure for altitude calculation by averaging
 * temperature and pressure measurements.
 *
 * @param BMP_data: pointer to a BMP280 structure.
 * @param samples: Number of measurements to average..
 * @param delay: Delay between measurements (in ms).
 *
 * @retval 0 OK
 * @retval -1 Error
 * */
int8_t BMP280_MeasureReference(BMP280 *BMP_data, uint16_t samples, uint8_t delay) {
	float sum = 0;
	for (char i = 0; i < samples; i++)	{
		// Update values
		if (BMP280_ReadTemperature(BMP_data) != 0) {
			return -1; // Error
			// continue instead ?
		}
		if (BMP280_ReadPressure(BMP_data) != 0) {
			return -1; // Error
			// continue instead ?
		}
		sum += BMP_data->press_Pa;
		HAL_Delay(delay);
	}
	BMP_data->press_ref_Pa = sum / samples;

	return 0; // OK
}

/**
 * Reads temperature registers, then calculate temperature in Celsius from calibration data.
 *
 * Takes 0.05ms to complete
 *
 * @param BMP_data: pointer to a BMP280 structure.
 *
 * @retval 0 OK
 * @retval -1 ERROR
 */
int8_t BMP280_ReadTemperature(BMP280 *BMP_data) {
    // Read BMP280 adc value
    if (BMP280_Read(BMP280_REG_TEMP_MSB, BMP_RX_Buffer, 3) != 0) {
    	return -1; // SPI Error
    }
    int32_t adc_T = (BMP_RX_Buffer[0] << 12) | (BMP_RX_Buffer[1] << 4) | ((BMP_RX_Buffer[2] >> 4) & 0x0F);

    // 0x80000 means no measurement (3.3.2 Temperature measurement - BMP280 Datasheet)
    if (adc_T == 0x80000) {
    	return -1; // Error no measurement
    }

    int32_t var1 = ((((adc_T >> 3) - ((int32_t)BMP_data->calib_data.dig_T1 << 1))) * ((int32_t)BMP_data->calib_data.dig_T2)) >> 11;
    int32_t var2 = (((((adc_T >> 4) - ((int32_t)BMP_data->calib_data.dig_T1)) * ((adc_T >> 4) - ((int32_t)BMP_data->calib_data.dig_T1))) >> 12) * ((int32_t)BMP_data->calib_data.dig_T3)) >> 14;
    BMP_data->t_fine = var1 + var2;

    int32_t T = (BMP_data->t_fine * 5 + 128) >> 8;
    BMP_data->temp_C = (float)(T / 100.0); // + devBMP->temperature_ref;

    return 0; // OK
}

/**
 * Reads pressure registers, then calculate pressure in Pascal from calibration data.
 *
 * Takes 0.05ms to complete
 *
 * @param BMP_data: pointer to a BMP280 structure.
 *
 * @retval 0 OK
 * @retval -1 ERROR
 */
int8_t BMP280_ReadPressure(BMP280 *BMP_data) {
    // Read BMP280 adc value
	if (BMP280_Read(BMP280_REG_PRESS_MSB, BMP_RX_Buffer, 3) != 0) {
		return -1; // SPI Error
	}
    int32_t adc_P = (BMP_RX_Buffer[0] << 12) | (BMP_RX_Buffer[1] << 4) | ((BMP_RX_Buffer[2] >> 4) & 0x0F);

    // 0x80000 means no measurement (3.3.1 Pressure measurement - BMP280 Datasheet)
    if (adc_P == 0x80000) {
    	return -1; // Error no measurement
    }

    int64_t var1 = ((int64_t)BMP_data->t_fine) - 128000;
    int64_t var2 = var1 * var1 * (int64_t)BMP_data->calib_data.dig_P6;
    var2 = var2 + ((var1 * (int64_t)BMP_data->calib_data.dig_P5) << 17);
    var2 = var2 + (((int64_t)BMP_data->calib_data.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)BMP_data->calib_data.dig_P3) >> 8) + ((var1 * (int64_t)BMP_data->calib_data.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)BMP_data->calib_data.dig_P1) >> 33;
    if (var1 == 0) {
        return -1; // Error zero division
    }
    int64_t p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)BMP_data->calib_data.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)BMP_data->calib_data.dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)BMP_data->calib_data.dig_P7) << 4);
    p = (uint32_t)p;

    BMP_data->press_Pa = (float)p / (float)(1 << 8);

    return 0; // OK
}

/**
 * Reads temperature and pressure values from the BMP280,
 * then calculate the altitude from them.
 *
 * Takes 0.3ms to complete
 *
 * @param BMP_data: pointer to a BMP280 structure.
 *
 * @retval 0 OK
 * @retval -1 ERROR
 */
int8_t BMP280_ReadAltitude(BMP280 *BMP_data) {
	// Update values
	if (BMP280_ReadTemperature(BMP_data) != 0) {
		return -1; // Error
	}
	if (BMP280_ReadPressure(BMP_data) != 0) {
		return -1; // Error
	}

	BMP_data->alt_m = BMP280_PressureToAltitude(BMP_data->press_Pa, BMP_data->press_ref_Pa);

	return 0; // OK
}

/**
 * Get the altitude from a pressure value using a formula from Bosch Q&A
 *
 * Takes 0.2ms to complete
 *
 * @param pressure: Current pressure in Pascal.
 * @param pressure_ref: Reference pressure in Pascal.
 *
 * @return Altitude in meters
 */
float BMP280_PressureToAltitude(float pressure, float pressure_ref) {
	// https://community.bosch-sensortec.com/t5/Question-and-answers/How-to-calculate-the-altitude-from-the-pressure-sensor-data/qaq-p/5702
    return 44330 * (1.0 - pow(pressure / pressure_ref, 0.1903));
}

/**
 * Soft reset BMP280
 * "The device is reset using the complete power-on-reset procedure"
 * "The readout value is always 0x00"
 *
 * @retval 0 OK
 * @retval -1 SPI ERROR
 */
int8_t BMP280_SoftReset() {
    if (BMP280_Write(BMP280_REG_RESET, BMP280_RESET_VALUE) != 0) {
    	return -1; // SPI ERROR
    }
    HAL_Delay(2); // wait 2ms for bmp280 reset

    return 0; // OK
}

/**
 * Read BMP280 register using GPIO and SPI2 HAL functions
 *
 * @param reg: u8bit register address to read.
 * @param RX_Buffer: u8bit array to store X bytes of data.
 * @param size: Number of bytes to read.
 *
 * @retval 0 OK
 * @retval -1 SPI ERROR
 */
int8_t BMP280_Read(uint8_t reg, uint8_t RX_Buffer[], uint8_t size) {
    // Enable SPI communication with BMP280 by setting BMP280's Chip Select (CS) pin to LOW.
    HAL_GPIO_WritePin(BMP_CS_GPIO_Port, BMP_CS_Pin, GPIO_PIN_RESET);

    // Transmit Control byte (Read mode + Register address)
    reg |= 0x80; // Read mode
    if (HAL_SPI_Transmit(BMP_hspi, &reg, 1, BMP280_SPI_TIMEOUT) != HAL_OK) {
    	return -1; // SPI ERROR
    }

    // Receive Data byte
    if (HAL_SPI_Receive(BMP_hspi, RX_Buffer, size, BMP280_SPI_TIMEOUT) != HAL_OK) {
    	return -1; // SPI ERROR
    }

    // Disable SPI communication with BMP280 by setting BMP280's Chip Select (CS) pin to HIGH.
    HAL_GPIO_WritePin(BMP_CS_GPIO_Port, BMP_CS_Pin, GPIO_PIN_SET);

    return 0; // OK
}

/**
 * Write to a BMP280 register using GPIO and SPI2 HAL functions
 *
 * @param reg: u8bit register address to write.
 * @param data: u8bit data to write.
 *
 * @retval 0 OK
 * @retval -1 SPI ERROR
 */
int8_t BMP280_Write(uint8_t reg, uint8_t data) {
    // BMP_CS LOW
    HAL_GPIO_WritePin(BMP_CS_GPIO_Port, BMP_CS_Pin, GPIO_PIN_RESET);

    // Control byte (Write mode + Register address)
    reg &= ~0x80; // Write mode

    // Transmit Control byte and Data byte
    uint8_t TX_Buffer[2] = { reg, data };
    if (HAL_SPI_Transmit(BMP_hspi, TX_Buffer, 2, BMP280_SPI_TIMEOUT) != HAL_OK) {
    	return -1; // SPI ERROR
    }

    // BMP_CS HIGH
    HAL_GPIO_WritePin(BMP_CS_GPIO_Port, BMP_CS_Pin, GPIO_PIN_SET);

    return 0; // OK
}
