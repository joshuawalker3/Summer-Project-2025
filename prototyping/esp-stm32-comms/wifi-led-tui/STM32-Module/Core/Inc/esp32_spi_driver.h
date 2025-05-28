/*
 * esp32_spi_driver.h
 *
 *  Created on: May 28, 2025
 *      Author: joshuawalker
 */

#ifndef INC_ESP32_SPI_DRIVER_H_
#define INC_ESP32_SPI_DRIVER_H_

#include "main.h"

typedef struct {
	SPI_HandleTypeDef* spi_handle;
	uint16_t cs_pin;
	uint8_t cmd_buffer[1];
	uint8_t mosi_buffer[64];
	uint8_t miso_buffer[64];
} Esp32_SpiHandle;

void init_esp32_spihandle(Esp32_SpiHandle* handle, SPI_HandleTypeDef* spi_handle, uint16_t cs_pin);
HAL_StatusTypeDef esp32_spi_read_cmd(Esp32_SpiHandle* handle, uint32_t timeout);
HAL_StatusTypeDef esp32_spi_read_data(Esp32_SpiHandle* handle, uint16_t size, uint32_t timeout);
HAL_StatusTypeDef esp32_spi_send_data(Esp32_SpiHandle* handle, uint16_t size, uint32_t timeout);


#endif /* INC_ESP32_SPI_DRIVER_H_ */
