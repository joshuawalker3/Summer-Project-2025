#include "tcp_socket_driver.h"

#include "sys/socket.h"
#include "netdb.h"
#include "esp_log.h"

static const char* TAG = "tcp_socket_driver";

int tcp_create_and_connect(char* host_ip, int port) {
    int addr_family = 0;
    int ip_protocol = 0;

    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(host_ip);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    addr_family = AF_INET;
    ip_protocol = IPPROTO_IP;

    int sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return -1;
    }

    ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip, port);

    int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
        close(sock);
        return -2;
    }

    ESP_LOGI(TAG, "Successfully connected to %s:%d", host_ip, port);

    return sock;
}