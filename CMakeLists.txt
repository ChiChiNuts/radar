cmake_minimum_required(VERSION 3.16)

project(radar VERSION 0.1.0)

include(cmake/top.cmake)

include_directories(
    ${CMAKE_BINARY_DIR}/kconfig/include/generated
)


