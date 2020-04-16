cmake_minimum_required(VERSION 3.15)
project(TIN_Tablica_ogloszen)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wall -pthread")

#server
add_executable(server server.cpp RequestHandler.cpp RequestHandler.h)
add_executable(server_c TIN/sock_client1.c)