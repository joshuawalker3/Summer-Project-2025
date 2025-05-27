cmake_minimum_required(VERSION 3.30)
project(host)

set(CMAKE_CXX_STANDARD 14)

include_directories(nlohmann)

add_executable(host
        nlohmann/json.hpp
        nlohmann/json_fwd.hpp
        host.cpp)
