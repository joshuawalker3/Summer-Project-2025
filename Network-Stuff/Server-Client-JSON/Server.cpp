//
// Created by joshuawalker on 3/11/25.
//

#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char** argv) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd == -1) {
        std::cerr << "Socket creation failed\n";
        return 2;
    }

    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8000);

    if (bind(server_fd, (sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Fail to bind\n";
        close(server_fd);
        return 3;
    }

    if (listen(server_fd, 3)) {
        std::cerr << "Fail to listen\n";
        close(server_fd);
        return 4;
    }

    std::cout << "Listening on port 8000\n";

    while (true) {
        sockaddr_in client;
        socklen_t client_address_len = sizeof(client);
        int client_fd = accept(server_fd, (sockaddr*)&client, &client_address_len);

        if (client_fd < 0) {
            std::cerr << "Failed to make connection\n";
            continue;
        }

        char buffer[1024] = {0};
        int bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);

        if (bytes_received < 0) {
            std::cerr << "Failed to receive data\n";
            close(client_fd);
            continue;
        }

        std::cout << "Received " << buffer << std::endl;

        std::string response = "I received this";
        send(client_fd, response.c_str(), response.size(), 0);

        close(client_fd);
    }

    close(server_fd);
    return 0;
}