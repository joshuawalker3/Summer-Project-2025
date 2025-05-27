/* WiFi station Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "my_wifi.h"
#include "sys/socket.h"
#include "netdb.h"
#include "cJSON.h"
#include "driver/spi.h"
#include "driver/gpio.h"

/* The examples use WiFi configuration that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#ifndef MY_WIFI_
    #define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
    #define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#endif

#define EXAMPLE_ESP_MAXIMUM_RETRY  CONFIG_ESP_MAXIMUM_RETRY
#define SERVER_IP   "192.168.1.52"
#define SERVER_PORT 8000


/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

#define SLAVE_CS_PIN GPIO_NUM_15

static const char *TAG = "wifi station";

static const char* SPI_TAG = "spi comm";

static int s_retry_num = 0;

const uint32_t ON_CMD = 0x01;
const uint32_t OFF_CMD = 0x02;

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:%s",
                 ip4addr_ntoa(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    tcpip_adapter_init();

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS
        },
    };

    /* Setting a password implies station will connect to all security modes including WEP/WPA.
        * However these modes are deprecated and not advisable to be used. Incase your Access point
        * doesn't support WPA2, these mode can be enabled by commenting below line */

    if (strlen((char *)wifi_config.sta.password)) {
        wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler));
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler));
    vEventGroupDelete(s_wifi_event_group);
}

void minimal_spi_init(void) {
    ESP_LOGI(SPI_TAG, "Begin SPI init...");

    spi_config_t spi_config = {
        .interface.val = SPI_DEFAULT_INTERFACE,
        .intr_enable.val = SPI_MASTER_DEFAULT_INTR_ENABLE,
        .mode = SPI_MASTER_MODE,
        .clk_div = SPI_10MHz_DIV,
        .event_cb = NULL
    };

    esp_err_t ret = spi_init(HSPI_HOST, &spi_config);
    if (ret != ESP_OK) {
        ESP_LOGE(SPI_TAG, "SPI init failed: %d", ret);
    } else {
        ESP_LOGI(SPI_TAG, "SPI init success");
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

void socket_client_task(void *pvParameters)
{
    char rx_buffer[1024];
    char host_ip[] = SERVER_IP;
    int addr_family = 0;
    int ip_protocol = 0;

    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(host_ip);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(SERVER_PORT);
    addr_family = AF_INET;
    ip_protocol = IPPROTO_IP;

    int sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
        return;
    }
    ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip, SERVER_PORT);

    int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
        close(sock);
        vTaskDelete(NULL);
        return;
    }
    ESP_LOGI(TAG, "Successfully connected");

	uint8_t status = 0x00;

    ESP_LOGI(TAG, "Stack high water mark: %d", uxTaskGetStackHighWaterMark(NULL));

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
    ESP_ERROR_CHECK(nvs_flash_init());

    minimal_spi_init();

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();

    xTaskCreate(socket_client_task, "socket_client", 16384, NULL, 5, NULL);

}
