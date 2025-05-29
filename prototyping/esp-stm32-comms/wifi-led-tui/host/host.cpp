//
// Created by joshuawalker on 5/25/25.
//

#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "nlohmann/json.hpp"

void print_menu();

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

    uint8_t status_num = 0;
    sockaddr_in client;
    socklen_t client_address_len = sizeof(client);
    int client_fd = accept(server_fd, (sockaddr*)&client, &client_address_len);

    if (client_fd < 0) {
        std::cerr << "Failed to make connection\n";
        return 4;
    }

    while (true) {
        int choice = 0;
        bool valid_entry = false;

        nlohmann::json send_cmd;

        while (!valid_entry) {
            print_menu();

            std::cin >> choice;

            choice--;

            switch (choice) {
                case 0:
                    valid_entry = true;

                    send_cmd["CMD"] = 0x00;

                    break;
                case 1:
                    valid_entry = true;

                    send_cmd["CMD"] = 0x01;

                    break;
                case 2:
                    valid_entry = true;

                    send_cmd["CMD"] = 0x02;

                    break;
                default:
                    std::cout << "Please enter a valid option\n";
                    break;
            }
        }

        std::string cmd_json = send_cmd.dump();

        std::cout << "Sending to ESP " << cmd_json << std::endl;

        send(client_fd, cmd_json.c_str(), cmd_json.size(), 0);

        std::cout << "Waiting on ESP..." << std::endl;
        char buffer[1024] = {0};
        int bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);

        if (bytes_received < 0) {
            std::cerr << "Failed to receive data\n";
            close(client_fd);
            break;
        }

        std::cout << "Received " << buffer << std::endl;

        nlohmann::json rec_json;

        try {
            rec_json = nlohmann::json::parse(buffer);
        } catch (nlohmann::json::parse_error e) {
            std::cerr << "Error parsing JSON\n";

            close(client_fd);

            break;
        }

        uint8_t success_bit;

        if (rec_json.contains("SUCCESS")) {
            std::string success = rec_json["SUCCESS"].get<std::string>();

            success_bit = static_cast<uint8_t>(std::stoi(success));
        } else {
            std::cout << "Unknown response received\n";

            continue;
        }

        if (!success_bit) {
            std::string err = rec_json["ERROR"].get<std::string>();

            std::cout << "Received error from esp32: " << err << std::endl;

            continue;
        }

        if (rec_json.contains("STATUS")) {
            std::string led_status = rec_json["STATUS"].get<std::string>();

            std::cout << "Current status of LED: " << led_status << std::endl;
        }
    }

    if (client_fd) {
        close(client_fd);
    }
    
    close(server_fd);
    return 0;
}

void print_menu() {
    std::cout << "1) Turn off LED\n"
                 "2) Turn on LED\n"
                 "3) Get status of LED"
                 "Enter the number of the menu choice you'd like to perform: " << std::endl;
}