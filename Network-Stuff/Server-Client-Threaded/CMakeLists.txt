cmake_minimum_required(VERSION 3.22)
project(Server_Client_Threaded)

set(CMAKE_CXX_STANDARD 14)

include_directories(includes)
include_directories(includes/nlohmann)

add_executable(Client.o
        includes/nlohmann/json.hpp
        includes/nlohmann/json_fwd.hpp
        includes/ClientProgram.h
        src/ClientProgram.cpp
        src/Client.cpp)

add_executable(Server.o
        includes/nlohmann/json.hpp
        includes/nlohmann/json_fwd.hpp
        includes/ClientHandler.h
        src/ClientHandler.cpp
        src/Server.cpp)