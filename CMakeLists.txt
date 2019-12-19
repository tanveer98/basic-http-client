cmake_minimum_required(VERSION 3.15)
project(basic_http_client)

set(CMAKE_CXX_STANDARD 14)

add_executable(basic_http_client main.cpp http_client.cpp http_client.h)