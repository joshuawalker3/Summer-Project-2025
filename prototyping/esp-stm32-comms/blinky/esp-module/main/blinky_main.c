#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define SLAVE_CS_PIN GPIO_NUM_15

void select_slave(gpio_num_t slave_cs);
void unselect_slave(gpio_num_t slave_cs);
esp_err_t master_transmit(uint32_t* command);

const uint32_t ON_CMD = 0x01;
const uint32_t OFF_CMD = 0x02;

const char *TAG = "spi_comm";

void app_main(void)
{
    //uint32_t zero = 0xA5A5A5A5;

    ESP_LOGI(TAG, "Starting minimal init");

    vTaskDelay(pdMS_TO_TICKS(1000));

    spi_config_t spi_config = {
        .interface.val = SPI_DEFAULT_INTERFACE,
        .intr_enable.val = SPI_MASTER_DEFAULT_INTR_ENABLE,
        .mode = SPI_MASTER_MODE,
        .clk_div = SPI_10MHz_DIV,
        .event_cb = NULL
    };

    esp_err_t ret = spi_init(HSPI_HOST, &spi_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI init failed: %d", ret);
    } else {
        ESP_LOGI(TAG, "SPI init success");
    }

    while (true) {
        ESP_LOGI(TAG, "Transmitting ON CMD...");
        select_slave(SLAVE_CS_PIN);
		esp_err_t status = master_transmit(&ON_CMD);

        if (status != ESP_OK) {
            ESP_LOGE(TAG, "ON Transmission failed");
        } else {
            ESP_LOGI(TAG, "ON Transmitted!");
        }
        unselect_slave(SLAVE_CS_PIN);

		vTaskDelay(pdMS_TO_TICKS(1000));

        ESP_LOGI(TAG, "Transmitting OFF CMD...");
        select_slave(SLAVE_CS_PIN);
		status = master_transmit(&OFF_CMD);

        if (status != ESP_OK) {
            ESP_LOGE(TAG, "OFF Transmission failed");
        } else {
            ESP_LOGI(TAG, "OFF Transmitted");
        }
        unselect_slave(SLAVE_CS_PIN);

        ESP_LOGI(TAG, "Looping...");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void select_slave(gpio_num_t slave_cs) {
    gpio_set_level(slave_cs, 0);
}

void unselect_slave(gpio_num_t slave_cs) {
    gpio_set_level(slave_cs, 1);
}

esp_err_t master_transmit(uint32_t* command) {
    spi_trans_t trans = {0};

    trans.cmd = NULL;
    trans.addr = NULL;
    trans.bits.cmd = 0;
    trans.bits.addr = 0;

    trans.bits.mosi = 8;         
    trans.mosi = command;         

    return spi_trans(HSPI_HOST, &trans);
}
