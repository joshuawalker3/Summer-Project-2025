cmake_minimum_required(VERSION 3.30)
project(basic_gui C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)

set(CMAKE_C_STANDARD 11)

add_executable(basic_gui
        basic_gui.c)

target_link_libraries(basic_gui PRIVATE PkgConfig::GTK4)