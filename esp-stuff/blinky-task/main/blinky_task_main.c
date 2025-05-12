#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define STACK_SIZE_MIN 512

void blinky(void* args) {
	printf("Begin Blinky\n");

	gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);

    while (true) {
        printf("LED ON\n");
        gpio_set_level(GPIO_NUM_2, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        printf("LED OFF\n");
        gpio_set_level(GPIO_NUM_2, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main() {
	xTaskCreate(blinky, "Blinky", STACK_SIZE_MIN, NULL, tskIDLE_PRIORITY + 1, NULL);
}
