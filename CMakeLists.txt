cmake_minimum_required(VERSION 3.10)
project(hw1 C)

set(CMAKE_C_STANDARD 99)

add_executable(client.o client.c)
add_executable(server.o server.c)