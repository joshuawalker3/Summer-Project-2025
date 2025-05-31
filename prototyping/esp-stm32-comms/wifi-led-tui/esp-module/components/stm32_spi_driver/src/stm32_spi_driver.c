#include "stm32_spi_driver.h"

#include "driver/spi.h"
#include "esp_log.h"

static const char* TAG = "stm32_spi_driver";

void minimal_spi_init(void) {
    ESP_LOGI(TAG, "Begin SPI init...");

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
}

void select_slave(gpio_num_t slave_cs) {
    gpio_set_level(slave_cs, 0);
}

void unselect_slave(gpio_num_t slave_cs) {
    gpio_set_level(slave_cs, 1);
}

esp_err_t master_transmit(uint8_t* command) {
    spi_trans_t trans = {0};

    trans.cmd = NULL;
    trans.addr = NULL;
    trans.bits.cmd = 0;
    trans.bits.addr = 0;

    trans.bits.mosi = 8;         
    trans.mosi = command;         

    return spi_trans(HSPI_HOST, &trans);
}

esp_err_t master_read(uint8_t* data, uint16_t size) {
    spi_trans_t trans = {0};

    trans.cmd = NULL;
    trans.addr = NULL;
    trans.bits.cmd = 0;
    trans.bits.addr = 0;

    trans.bits.miso = size * 8;         
    trans.miso = data;         

    return spi_trans(HSPI_HOST, &trans);
}