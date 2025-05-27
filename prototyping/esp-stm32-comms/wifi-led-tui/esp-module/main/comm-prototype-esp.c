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

	uint8_t status = 0x00;

    while (true) {
        cJSON* root = cJSON_CreateObject();

        if (root == NULL) {
            ESP_LOGI(TAG, "root null");
        }

        char status_send[4];
        snprintf(status_send, sizeof(status_send), "%u", status);

    	if (!cJSON_AddStringToObject(root, "STATUS", status_send)) {
            ESP_LOGI(TAG, "Failed to add status");
        }

    	char* payload = cJSON_PrintUnformatted(root);

        if (payload == NULL) {
            ESP_LOGI(TAG, "Payload null");
        }

        ESP_LOGI(TAG, "Sending %s", payload);
    	send(sock, payload, strlen(payload), 0);

        cJSON_free(payload);
        cJSON_Delete(root);

    	int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);

    	if (len < 0) {
        	ESP_LOGE(TAG, "recv failed: errno %d", errno);
    	} else {
        	rx_buffer[len] = '\0';

        	ESP_LOGI(TAG, "Received: %s", rx_buffer);

			cJSON* received = cJSON_Parse(rx_buffer);

            uint8_t cmd = (uint8_t)(cJSON_GetObjectItem(received, "CMD")->valueint);

            ESP_LOGI(TAG, "Received cmd %d", cmd);

            cJSON_Delete(received);

            select_slave(SLAVE_CS_PIN);
            master_transmit(&cmd);
            unselect_slave(SLAVE_CS_PIN);
    	}

        status ^= 0x01;

        vTaskDelay(1000 /  portTICK_PERIOD_MS);
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
