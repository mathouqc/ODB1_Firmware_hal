/*
 * L76LM33.c
 *
 * L76LM33 is a GNSS module used to get the position (latitude, longitude) of the rocket.
 *
 * This module handles reading UART to receive NMEA sentence, then it parses the sentence
 * using the NMEA module into a structure.
 *
 * Store UART received byte via interrupts into circular buffer using L76LM33_RxCallback()
 * Read circular buffer to find NMEA sentence and parse it using L76LM33_Read()
 *
 *  Created on: May 12, 2024
 *      Author: gagnon
 *
 *  Edited on: Jul 4, 2024
 *      Autor: mathouqc
 */

#include "GAUL_Drivers/L76LM33.h"

#include "GAUL_Drivers/NMEA.h"
#include "ringbuffer.h"

// Pointer to UART handler
UART_HandleTypeDef *L76_huart;

// Received char/byte from UART
uint8_t L76_receivedByte;

// Circular buffer to store UART data from GNSS module
ring_buffer_t L76_UART_Buffer;
char L76_UART_Buffer_arr[L76LM33_BUFFER_SIZES];

// 1: line is available in UART buffer, 0: line is not available in UART buffer
uint8_t L76_new_line = 0;

// Buffer to store NMEA sentence
uint8_t L76_NMEA_Buffer[L76LM33_BUFFER_SIZES];

// Struct to store parsed NMEA data
GPS_Data L76_gps_data;


/*
 * Source:
 * LG76 Series GNSS Protocol Specification - Section 2.3. PMTK Messages
 *
 * Only output RMC (Recommended Minimum Specific GNSS Sentence) once every one position fix
 * NMEA_RMC[] = "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*35<CR><LF>"
 *
 * Set the navigation mode to "Aviation Mode" (for large acceleration movement, altitude of 10'000m max)
 * NMEA_NAVMODE = "PMTK886,2*2A<CR><LF>"
 */

/**
 * Initialize L76LM33 sensor.
 *
 * @param L76_data: pointer to a L76LM33 structure.
 * @param huart: pointer to a HAL UART handler.
 *
 * @retval 0 OK
 * @retval -1 ERROR
 */
int8_t L76LM33_Init(UART_HandleTypeDef *huart) {
	// Set UART handler
	L76_huart = huart;

	// Initialize circular buffer
	ring_buffer_init(&L76_UART_Buffer, L76_UART_Buffer_arr, sizeof(L76_UART_Buffer_arr));

	// Receive UART data with interrupts
	if (HAL_UART_Receive_IT(L76_huart, &L76_receivedByte, 1) != HAL_OK) {
		return -1; // Error with UART
	}

	// Only output GPRMC sentence
    char NMEA_RMC[] = "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*35\r\n";
    if (L76LM33_SendCommand(NMEA_RMC, sizeof(NMEA_RMC)) != 0) {
    	return -1; // Error with UART
    }
    // Set navigation mode
    char NMEA_NAVMODE[] = "PMTK886,2*2A\r\n";
    if (L76LM33_SendCommand(NMEA_NAVMODE, sizeof(NMEA_NAVMODE)) != 0) {
    	return -1; // Error with UART
    }

    return 0; // OK
}

/**
 * Callback called on incoming UART data. It is called when HAL_UART_RxCpltCallback is called in main.c.
 * Add received byte to UART circular buffer.
 *
 * @param huart: pointer to a HAL UART handler triggering the callback
 */
void L76LM33_RxCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == L76_huart->Instance) {
		// Add data to circular buffer
		ring_buffer_queue(&L76_UART_Buffer, L76_receivedByte);
		if (L76_receivedByte == '\n') {
			L76_new_line = 1;
		}
		// Receive UART data with interrupts
		HAL_UART_Receive_IT(L76_huart, &L76_receivedByte, 1);
	}
}

/**
 * Read and parse a NMEA GPRMC sentence into data structure. Call this function
 * frequently to have the latest GPS data available.
 *
 * @param L76_data: pointer to a L76LM33 structure to update.
 *
 * @retval 0 OK
 * @retval -1 ERROR
 * @retval -2 Empty UART buffer, struct unchanged
 *
 */
int8_t L76LM33_Read(L76LM33 *L76_data) {
	// Read sentence
	int8_t valid = L76LM33_ReadSentence();
	if (valid == -2) {
		// Empty buffer, don't change structure
		return -2;
	} else if (valid != 0) {
		L76_data->status = 0; // Bad status
		return -1; // Error
	}

	// Validate sentence ID is RMC
	if (NMEA_ValidateRMC((char *)L76_NMEA_Buffer) != 0) {
		L76_data->status = 0; // Bad status
		return -1; // Error, sentence ID is not RMC
	}

	// Parse NMEA RMC sentence to local structure
	if (NMEA_ParseRMC((char *)L76_NMEA_Buffer, &L76_gps_data) != 0) {
		L76_data->status = 0; // Bad status
		return -1;
	}

	// Fill L76LM33 structure
	L76_data->fix = L76_gps_data.fix;
	L76_data->longitude = L76_gps_data.longitude;
	L76_data->latitude = L76_gps_data.latitude;

	L76_data->status = 1; // Good status (no error)
	return 0; // OK
}

/**
 * Read NMEA sentence from UART circular buffer into a NMEA buffer.
 *
 * @retval 0 OK
 * @retval -1 Error, cannot find starting or ending character.
 * @retval -2 Error, empty UART buffer
 *
 */
int8_t L76LM33_ReadSentence() {
	if (L76_new_line == 0) {
		return -2; // Error, empty UART circular buffer
	}

	// Reset flag
	L76_new_line = 0;

	// Clear NMEA buffer
	for (int16_t i = 0; i < sizeof(L76_NMEA_Buffer); i++) {
		L76_NMEA_Buffer[i] = 0;
	}


	// Variable to store character from UART buffer
	char c;


	// Try to find '$' in 100 iterations
	for (uint16_t i = 0; i < 100; i++) {
		// Read character from UART buffer
		if (ring_buffer_dequeue(&L76_UART_Buffer, &c) != 1) {
			return -2; // Error, empty buffer
		}

		if (c == '$') {
			// Set starting character in NMEA buffer
			L76_NMEA_Buffer[0] = '$';

			break; // Found starting characters
		}
	}

	if (c != '$') {
		return -1; // Error, cannot find starting character in 100 iterations
	}


	// Read into NMEA buffer until ending character is found
	for (uint16_t i = 1; i < sizeof(L76_NMEA_Buffer); i++) {
		// Read character from UART buffer
		if (ring_buffer_dequeue(&L76_UART_Buffer, &c) != 1) {
			return -2; // Error, empty buffer
		}

		// Add character to NMEA buffer
		L76_NMEA_Buffer[i] = c;

		if (c == '\n') {
			break; // Found ending character
		}
	}

	if (c != '\n') {
		return -1; // Error, cannot find '\n'
	}

	return 0;
}

/**
 * Send array of character to L76LM33 using UART HAL functions.
 *
 * @param command[]: array of character to send.
 * @param size: size of the data to send.
 *
 * @retval 0 OK
 * @retval -1 ERROR
 */
int8_t L76LM33_SendCommand(char command[], uint8_t size) {
    if (command == NULL) {
        return -1; // Error
    }

    if (HAL_UART_Transmit(L76_huart, (uint8_t *)command, size, L76LM33_UART_TIMEOUT) != HAL_OK) {
    	return -1; // Error with UART
    }

    return 0; // OK
}
