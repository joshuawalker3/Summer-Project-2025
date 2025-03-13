//
// Created by joshuawalker on 3/12/25.
//

#ifndef SERVER_CLIENT_THREADED_CLIENTPROGRAM_H
#define SERVER_CLIENT_THREADED_CLIENTPROGRAM_H

#include <sys/socket.h>

class ClientProgram {
private:
    int socket_fd;
    char buffer[1024];
    bool cont;

public:
    ClientProgram();

    ~ClientProgram();

    void run();

    bool connectToServer();
};


#endif //SERVER_CLIENT_THREADED_CLIENTPROGRAM_H
