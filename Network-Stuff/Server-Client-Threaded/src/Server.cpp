//
// Created by joshuawalker on 3/12/25.
//

#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "nlohmann/json.hpp"
#include "ClientHandler.h"
#include <thread>
#include <csignal>

int server_fd;
ClientHandler* clientHandler;

void signalHandler(int signum) {
    close(server_fd);

    delete clientHandler;

    std::exit(signum);
}

int main(int argc, char** argv) {
    std::signal(SIGINT, signalHandler);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

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

    std::vector<std::thread> threads;

    while (true) {
        clientHandler = new ClientHandler();

        bool connected = clientHandler->connectToClient(server_fd);

        if (connected) {
            std::thread worker(&ClientHandler::start, clientHandler);
            worker.detach();
        } else {
            delete clientHandler;
        }
    }

    return 0;
}