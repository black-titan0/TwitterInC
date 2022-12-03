cmake_minimum_required(VERSION 3.17)
project(Client C)

set(CMAKE_C_STANDARD 11)

add_executable(Client main.c cJSON.c cJSON.h myGUI.c myGUI.h myNetwork.c myNetwork.h)

target_link_libraries(Client ws2_32)