cmake_minimum_required(VERSION 3.15)
project(basic_http_client)

set(CMAKE_CXX_STANDARD 20)


add_executable(basic_http_client main.cpp basic_http_client.h src/client/HttpClient.cpp src/client/HttpClient.h src/cmdline/Cmdline.cpp src/cmdline/Cmdline.h src/client/request_header/Request_Header.cpp src/client/request_header/Request_Header.h src/client/https-client/HttpsClient.cpp src/client/https-client/HttpsClient.h)
target_link_libraries(basic_http_client -lssl -lcrypto -ltls -L/usr/local/lib)