cmake_minimum_required(VERSION 3.30)
project(example_0 C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)

set(CMAKE_C_STANDARD 11)

add_executable(example_0
        example0.c)

target_link_libraries(example_0 PRIVATE PkgConfig::GTK4)
