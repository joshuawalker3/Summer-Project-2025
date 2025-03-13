//
// Created by joshuawalker on 3/12/25.
//

#include "ClientHandler.h"

#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "nlohmann/json.hpp"

ClientHandler::ClientHandler() {
    this->client_fd = 0;
    cont = true;
    memset(this->buffer, 0, 1024);
}

ClientHandler::~ClientHandler() {
    close(this->client_fd);
}

void ClientHandler::start() {
    while (this->cont) {
        memset(this->buffer, 0, 1024);

        nlohmann::json req = getRequest();

        if (req == NULL) {
            sendError("Failed to process request");

            continue;
        }

        if (!req.contains("OPERATION")) {
            sendError("Invalid request");

            continue;
        }

        int op = req["OPERATION"].get<int>();

        std::vector<nlohmann::json> numbers;

        if (!req["NUMBERS"].is_null()) {
            numbers = req["NUMBERS"].get<std::vector<nlohmann::json>>();
        }

        int result = 0;

        switch (op) {
            case 0:
                std::cout << "Client exited" << std::endl;
                this->cont = false;
                break;
            case 1:
                result = numbers[0]["NUM1"].get<int>() + numbers[1]["NUM2"].get<int>();

                sendResult(result);
                break;
            case 2:
                result = numbers[1]["NUM2"].get<int>() - numbers[0]["NUM1"].get<int>();

                sendResult(result);
                break;
            default:
                sendError("Unknown operation requested");
                break;
        }
    }

    delete this;
}

nlohmann::json ClientHandler::getRequest() {
    ssize_t bytes_received = recv(client_fd, this->buffer, sizeof(this->buffer), 0);

    if (bytes_received < 0) {
        std::cerr << "Failed to receive data\n";

        return NULL;
    }

    nlohmann::json rec_json;

    try {
        rec_json = nlohmann::json::parse(buffer);
    } catch (const nlohmann::json::parse_error &e) {
        std::cerr << "Error parsing JSON\n";

        return NULL;
    }

    return rec_json;
}

void ClientHandler::sendResult(int result) {
    nlohmann::json resp;

    resp["SUCCESS"] = true;
    resp["RESULT"] = result;

    std::string response = resp.dump();
    send(client_fd, response.c_str(), response.size(), 0);
}

void ClientHandler::sendError(std::string msg) {
    nlohmann::json resp;

    resp["SUCCESS"] = false;
    resp["ERROR"] = msg;

    std::string response = resp.dump();
    send(client_fd, response.c_str(), response.size(), 0);
}

bool ClientHandler::connectToClient(int server_fd) {
    sockaddr_in client;
    socklen_t client_address_len = sizeof(client);
    this->client_fd = accept(server_fd, (sockaddr*)&client, &client_address_len);

    if (this->client_fd < 0) {
        std::cerr << "Failed to make connection\n";
        return false;
    }

    return true;
}

bool ClientHandler::getCont() {
    return this->cont;
}