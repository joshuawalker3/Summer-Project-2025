/*
 * esp32_spi_driver.c
 *
 *  Created on: May 28, 2025
 *      Author: joshuawalker
 */

#include "esp32_spi_driver.h"

void init_esp32_spihandle(Esp32_SpiHandle* handle, SPI_HandleTypeDef* spi_handle, uint16_t cs_pin) {
	handle->spi_handle = spi_handle;
	handle->cs_pin = cs_pin;

	handle->miso_buffer[0] = 0x00;

	esp32_spi_send_data(handle, 1, 100);
}

HAL_StatusTypeDef esp32_spi_read_cmd(Esp32_SpiHandle* handle, uint32_t timeout) {
	if (!handle->spi_handle) {
		return HAL_ERROR;
	}

	return HAL_SPI_Receive(handle->spi_handle, handle->cmd_buffer, sizeof(uint8_t), timeout);
}

HAL_StatusTypeDef esp32_spi_read_data(Esp32_SpiHandle* handle, uint16_t size, uint32_t timeout) {
	if (!handle->spi_handle) {
		return HAL_ERROR;
	}

	return HAL_SPI_Receive(handle->spi_handle, handle->mosi_buffer, size, timeout);
}

HAL_StatusTypeDef esp32_spi_send_data(Esp32_SpiHandle* handle, uint16_t size, uint32_t timeout) {
	if (!handle->spi_handle) {
		return HAL_ERROR;
	}

	return HAL_SPI_Transmit(handle->spi_handle, handle->miso_buffer, size, timeout);
}
