# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(EXTRA_COMPONENT_DIRS "components/my_wifi_driver"
						 "components/tcp_socket_driver"
						 "components/stm32_spi_driver")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(comm-prototype-esp)
