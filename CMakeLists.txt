cmake_minimum_required(VERSION 3.15)
project(basic_http_client)

set(CMAKE_CXX_STANDARD 20)


add_executable(basic_http_client main.cpp basic_http_client.h HttpClient.cpp HttpClient.h)
target_link_libraries(basic_http_client -lssl -lcrypto -ltls -L/usr/local/lib)