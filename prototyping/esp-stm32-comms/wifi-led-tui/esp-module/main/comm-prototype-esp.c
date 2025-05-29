/* WiFi station Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "my_wifi_driver.h"
#include "tcp_socket_driver.h"
#include "stm32_spi_driver.h"

//#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"

//#include "esp_netif.h"
//#include "esp_event.h"
//#include "esp_wifi.h"
//#include "nvs.h"
//#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "sys/socket.h"
#include "netdb.h"
#include "cJSON.h"
#include "driver/spi.h"
#include "driver/gpio.h"

static const char *TAG = "main";

const uint32_t ON_CMD = 0x01;
const uint32_t OFF_CMD = 0x02;

#define HOST_IP "192.168.1.52"
#define HOST_PORT 8000

void socket_client_task(void *pvParameters)
{
    char rx_buffer[1024];

    int sock = tcp_create_and_connect(HOST_IP, HOST_PORT);

    if (sock < 0) {
        vTaskDelete(NULL);
    }

    while (true) {
        int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);

        if (len < 0) {
            ESP_LOGE(TAG, "recv failed: errno %d", errno);

            break;
        }

        rx_buffer[len] = '\0';

        ESP_LOGI(TAG, "Received: %s", rx_buffer);

        cJSON* received = cJSON_Parse(rx_buffer);

        uint8_t cmd = (uint8_t)(cJSON_GetObjectItem(received, "CMD")->valueint);

        ESP_LOGI(TAG, "Received cmd %d", cmd);

        cJSON_Delete(received);

        cJSON* root = cJSON_CreateObject();

        esp_err_t err;

        uint8_t success = 0x01;
        char success_bit[2];

        switch(cmd) {
        case 0x00:
            select_slave(SLAVE_CS_PIN);
            err = master_transmit(&cmd);
            unselect_slave(SLAVE_CS_PIN);

            if (err != ESP_OK) {
                success = 0x00;

                cJSON_AddStringToObject(root, "ERROR", esp_err_to_name(err));

                ESP_LOGE(TAG, "Error while processing cmd 0x00: %s", esp_err_to_name(err));
            }

            break;
        case 0x01:
            select_slave(SLAVE_CS_PIN);
            err = master_transmit(&cmd);
            unselect_slave(SLAVE_CS_PIN);

            if (err != ESP_OK) {
                success = 0x00;

                cJSON_AddStringToObject(root, "ERROR", esp_err_to_name(err));

                ESP_LOGE(TAG, "Error while processing cmd 0x01: %s", esp_err_to_name(err));
            }

            break;
        case 0x02: ; //null statement
            uint8_t led_status = 0x00;

            char led_status_str[4];

            select_slave(SLAVE_CS_PIN);
            err = master_transmit(&cmd);

            if (err != ESP_OK) {
                success = 0x00;

                cJSON_AddStringToObject(root, "ERROR", esp_err_to_name(err));

                ESP_LOGE(TAG, "Error while processing cmd 0x01: %s", esp_err_to_name(err));
            }

            err = master_read(&led_status, sizeof(uint8_t));

            if (err != ESP_OK) {
                success = 0x00;

                cJSON_AddStringToObject(root, "ERROR", esp_err_to_name(err));

                ESP_LOGE(TAG, "Error while reading from slave: %s", esp_err_to_name(err));
            }

            unselect_slave(SLAVE_CS_PIN);

            snprintf(led_status_str, sizeof(led_status_str), "%u", led_status);

            cJSON_AddStringToObject(root, "STATUS", led_status_str);

            break;
        default:
            success = 0x00;

            cJSON_AddStringToObject(root, "ERROR", "Unknown cmd received");

            break;
        }

        snprintf(success_bit, sizeof(success_bit), "%u", success);

        cJSON_AddStringToObject(root, "SUCCESS", success_bit);

        char* payload = cJSON_PrintUnformatted(root);

        ESP_LOGI(TAG, "Sending %s", payload);
        send(sock, payload, strlen(payload), 0);

        cJSON_free(payload);
        cJSON_Delete(root);
    }

    shutdown(sock, 0);
    close(sock);
    vTaskDelete(NULL);
}



void app_main()
{
    initialize_nvs_flash();

    minimal_spi_init();

    wifi_init_station();

    xTaskCreate(socket_client_task, "socket_client", 16384, NULL, 5, NULL);

}
