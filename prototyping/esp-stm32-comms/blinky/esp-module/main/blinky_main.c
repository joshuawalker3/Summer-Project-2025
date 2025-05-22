#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi.h"
#include "driver/gpio.h"

#define SLAVE_CS_PIN GPIO_NUM_16

void select_slave(gpio_num_t slave_cs);
void unselect_slave(gpio_num_t slave_cs);
esp_err_t master_transmit(uint32_t* data, uint32_t len);

uint32_t on_cmd = 0x01;

void app_main(void)
{
    printf("Starting minimal init\n");

    spi_config_t spi_config = {
        .interface.val = SPI_DEFAULT_INTERFACE,
        .interface.cs_en = 0,           // Disable CS
        .intr_enable.val = 0,           // No interrupts
        .mode = SPI_MASTER_MODE,
        .clk_div = SPI_10MHz_DIV,
        .event_cb = NULL
    };

    esp_err_t ret = spi_init(HSPI_HOST, &spi_config);
    if (ret != ESP_OK) {
        printf("SPI init failed: %d\n", ret);
    } else {
        printf("SPI init success\n");
    }

    gpio_config_t io_15_conf = {
        .pin_bit_mask = 1 << 15,
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_15_conf);
    gpio_set_level(GPIO_NUM_15, 0);  

    gpio_config_t io_16_conf = {
        .pin_bit_mask = 1 << 16,
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_16_conf);
    unselect_slave(SLAVE_CS_PIN);

    while (true) {
		printf("Transmitting ON CMD....\n");
        select_slave(SLAVE_CS_PIN);
		esp_err_t status = master_transmit(&on_cmd, sizeof(on_cmd));

        if (status != ESP_OK) {
            printf("Transmission failed\n");
        } else {
            printf("Success!\n");
        }
        unselect_slave(SLAVE_CS_PIN);


        printf("Looping...\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void select_slave(gpio_num_t slave_cs) {
    gpio_set_level(slave_cs, 0);
}

void unselect_slave(gpio_num_t slave_cs) {
    gpio_set_level(slave_cs, 1);
}

esp_err_t master_transmit(uint32_t* data, uint32_t len) {
    spi_trans_t trans;
    uint16_t cmd; 
    uint32_t addr = 0;

	cmd = SPI_MASTER_WRITE_DATA_TO_SLAVE_CMD;
    trans.bits.mosi = len * 32;
    trans.mosi = data;

    trans.bits.cmd = 8;
    trans.bits.addr = 8;
    trans.cmd = &cmd;
    trans.addr = &addr;

    return spi_trans(HSPI_HOST, &trans);
}
