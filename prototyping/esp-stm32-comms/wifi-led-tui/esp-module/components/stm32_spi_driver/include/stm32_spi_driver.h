#ifndef STM32_SPI_DRIVER_
#define STM32_SPI_DRIVER_

#include "driver/gpio.h"

#define SLAVE_CS_PIN GPIO_NUM_15

void minimal_spi_init(void);

void select_slave(gpio_num_t slave_cs);

void unselect_slave(gpio_num_t slave_cs);

esp_err_t master_transmit(uint8_t* command);

esp_err_t master_read(uint8_t* data, uint16_t size);

#endif