//
// Created by joshuawalker on 3/11/25.
//

#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char** argv) {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        std::cerr << "Failed to create socket\n";
        return 2;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8000);
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) < 0) {
        std::cerr << "Failed to bind\n";
        close(socket_fd);
        return 3;
    }

    if (connect(socket_fd, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Failed to connect\n";
        close(socket_fd);
        return 4;
    }

    std::string message = "Did you receive this?";

    if (send(socket_fd, message.c_str(), message.size(), 0) == -1) {
        std::cerr << "Failed to send\n";
        close(socket_fd);
        return 5;
    }

    char buffer[1024] = {0};
    if (recv(socket_fd, buffer, sizeof(buffer), 0) == -1) {
        std::cerr << "Fail to receive\n";
        close(socket_fd);
        return 6;
    }

    std::cout << "Received " << buffer << std::endl;

    close(socket_fd);

    return 0;
}