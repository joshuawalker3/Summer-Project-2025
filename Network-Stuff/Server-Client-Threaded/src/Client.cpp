//
// Created by joshuawalker on 3/12/25.
//

#include <iostream>
#include "ClientProgram.h"

int main(int argc, char** argv) {
    auto clientProgram = new ClientProgram();

    bool connected = clientProgram->connectToServer();

    if (connected) {
        clientProgram->run();
    }

    delete clientProgram;

    return 0;
}