#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi.h"
#include "driver/gpio.h"

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

    // 🛠️ Manually reconfigure GPIO15 as OUTPUT LOW to prevent reboot issue
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << 15,
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    gpio_set_level(GPIO_NUM_15, 0);  // Force GPIO15 LOW

    while (true) {
        printf("Looping...\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
