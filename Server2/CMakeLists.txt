cmake_minimum_required(VERSION 3.17)
project(Server2 C)

set(CMAKE_C_STANDARD 11)

add_executable(Server2 main.c cJSON.c cJSON.h)

target_link_libraries(Server2 ws2_32)