//
// Created by joshuawalker on 3/12/25.
//

#include "ClientProgram.h"

#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "nlohmann/json.hpp"

ClientProgram::ClientProgram() {
    this->socket_fd = 0;
    this->cont = true;
    memset(this->buffer, 0, 1024);
}

ClientProgram::~ClientProgram() {
    close(this->socket_fd);
}

void ClientProgram::run() {
    while (cont) {
        std::cout << "What would you like to do?\n" <<
        "1) Add 2 numbers\n" <<
        "2) Subtract 2 numbers\n" <<
        "0) Quit\n\n";

        int choice;

        std::cin >> choice;

        nlohmann::json req;
        nlohmann::json numbers = nlohmann::json::array({});

        int nums[2];

        switch (choice) {
            case 0:
                req["OPERATION"] = 0;
                cont = false;
                break;
            case 1:
                std::cout << "Whats the first number to add? ";
                std::cin >> nums[0];
                std::cout << "Whats the second number to add? ";
                std::cin >> nums[1];

                for (int i = 0; i < 2; i++) {
                    nlohmann::json tmp;
                    tmp["NUM" + std::to_string(i + 1)] = nums[i];

                    numbers[i] = tmp;
                }

                req["OPERATION"] = 1;
                req["NUMBERS"] = numbers;

                break;
            case 2:
                std::cout << "Whats the first number to subtract? ";
                std::cin >> nums[0];
                std::cout << "Whats the second number to subtract? ";
                std::cin >> nums[1];

                for (int i = 0; i < 2; i++) {
                    nlohmann::json tmp;
                    tmp["NUM" + std::to_string(i + 1)] = nums[i];

                    numbers[i] = tmp;
                }

                req["OPERATION"] = 2;
                req["NUMBERS"] = numbers;

                break;
            default:
                std::cout << "Invalid choice entered!\n";
                continue;
        }

        std::string message = req.dump();

        if (send(this->socket_fd, message.c_str(), message.size(), 0) == -1) {
            std::cerr << "Failed to send\n";
            continue;
        }

        if (choice == 0) {
            continue;
        }

        if (recv(this->socket_fd, this->buffer, sizeof(this->buffer), 0) == -1) {
            std::cerr << "Fail to receive\n";
            continue;
        }

        nlohmann::json resp;

        try {
            resp = nlohmann::json::parse(this->buffer);
        } catch (nlohmann::json::parse_error e) {
            std::cerr << "Fail to parse JSON\n";
            continue;
        }

        if (!resp.contains("SUCCESS") || !resp["SUCCESS"].get<bool>()) {
            std::cerr << "Unknown response received: SUCCESS missing\n";
            continue;
        }

        if (resp.contains("RESULT")) {
            std::cout << "Result is " << resp["RESULT"].get<int>() << std::endl;
        } else {
            std::cerr << "Unknown response received: RESULT missing\n";
            continue;
        }

        memset(this->buffer, 0, 1024);
    }
}

bool ClientProgram::connectToServer() {
    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->socket_fd == -1) {
        std::cerr << "Failed to create socket\n";
        return false;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8000);
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) < 0) {
        std::cerr << "Failed to bind\n";
        close(this->socket_fd);
        return false;
    }

    if (connect(this->socket_fd, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Failed to connect\n";
        close(this->socket_fd);
        return false;
    }

    return true;
}