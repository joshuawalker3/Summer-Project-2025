//
// Created by joshuawalker on 3/11/25.
//

#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "nlohmann/json.hpp"

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

    nlohmann::json test_json;
    nlohmann::json array_test = nlohmann::json::array({});

    test_json["Test"] = "Hello Server!";
    test_json["Name"] = "Josh";

    for (int i = 0; i < 10; i++) {
        nlohmann::json tmp;

        tmp["Num"] = i;

        array_test[i] = tmp;
    }

    test_json["array"] = array_test;

    std::string message = test_json.dump();

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

    nlohmann::json resp;

    try {
        resp = nlohmann::json::parse(buffer);
    } catch (nlohmann::json::parse_error e) {
        std::cerr << "Fail to parse JSON\n";
        close(socket_fd);
        return 7;
    }

    if (resp.contains("Sum")) {
        std::cout << "Sum is " << resp["Sum"].get<int>() << std::endl;
    } else {
        std::cerr << "Unknown response received\n";
    }

    close(socket_fd);

    return 0;
}