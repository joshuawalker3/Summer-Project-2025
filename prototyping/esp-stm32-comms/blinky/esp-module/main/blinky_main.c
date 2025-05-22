#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define SLAVE_CS_PIN GPIO_NUM_16

void select_slave(gpio_num_t slave_cs);
void unselect_slave(gpio_num_t slave_cs);
esp_err_t master_transmit(uint32_t* data, uint32_t len);

const uint8_t ON_CMD = 0x01;
const uint8_t OFF_CMD = 0x02;

const char *TAG = "spi_comm";

void app_main(void)
{
    ESP_LOGI(TAG, "Starting minimal init");

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
        ESP_LOGE(TAG, "SPI init failed: %d", ret);
    } else {
        ESP_LOGI(TAG, "SPI init success");
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

esp_err_t master_transmit(uint8_t* data) {
    spi_trans_t trans;
    uint32_t addr = 0;

	trans.cmd = data;
    trans.bits.cmd = 8;

    trans.bits.addr = 0;
    trans.bits.mosi = 0;
    trans.addr = NULL;
    trans.mosi = NULL;

    return spi_trans(HSPI_HOST, &trans);
}
