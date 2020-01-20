cmake_minimum_required(VERSION 3.15)
project(basic_http_client)

#set(CMAKE_CXX_STANDARD 20)
#add_executable(basic_http_client main.cpp basic_http_client.h src/client/HttpClient.cpp src/client/HttpClient.h src/cmdline/Cmdline.cpp src/cmdline/Cmdline.h src/client/request_header/Request_Header.cpp src/client/request_header/Request_Header.h src/client/https-client/HttpsClient.cpp src/client/https-client/HttpsClient.h src/client/interface-io/IO_Functions.cpp src/client/interface-io/IO_Functions.h)
#target_link_libraries(basic_http_client -lcrypto -ltls -L/usr/local/lib)

#statically linked build script.
#find library esentially find the directory of the lib.
#on actual build it the ${value} is /path/to/lib       
find_library(LIB_TLS libtls.a /usr/local/lib/)
find_library(LIB_SSL libssl.a /usr/local/lib/)
find_library(LIB_CRYPTO libcrypto.a /usr/local/lib/)
add_executable(basic_http_client main.cpp basic_http_client.h src/client/HttpClient.cpp src/client/HttpClient.h src/cmdline/Cmdline.cpp src/cmdline/Cmdline.h src/client/request_header/Request_Header.cpp src/client/request_header/Request_Header.h src/client/https-client/HttpsClient.cpp src/client/https-client/HttpsClient.h src/client/interface-io/IO_Functions.cpp src/client/interface-io/IO_Functions.h)
target_link_libraries(basic_http_client -lpthread ${LIB_TLS} ${LIB_SSL} ${LIB_CRYPTO})