//
// Created by joshuawalker on 3/12/25.
//

#ifndef SERVER_CLIENT_THREADED_CLIENTHANDLER_H
#define SERVER_CLIENT_THREADED_CLIENTHANDLER_H

#include "nlohmann/json.hpp"


class ClientHandler {
private:
    int client_fd;
    bool cont;
    char buffer[1024];

    nlohmann::json getRequest();
    void sendResult(int result);
    void sendError(std::string msg);

public:
    ClientHandler();

    ~ClientHandler();

    void start();
    bool connectToClient(int server_fd);
    bool getCont();
};


#endif //SERVER_CLIENT_THREADED_CLIENTHANDLER_H
