//
// Created by tanveer on 20.12.19.
//

#ifndef BASIC_HTTP_CLIENT_HTTPCLIENT_H
#define BASIC_HTTP_CLIENT_HTTPCLIENT_H
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <chrono>

#include <string>

//TODO:: implement cstor and dstor
namespace basic_http_client {
    class HttpClient {
    private:
        int create_client_socket();
        int connect_server();
        int send_request();
        int async_socket();
        int create_tls();

        uint8_t *recv_response();
        std::chrono::steady_clock::time_point begin;
        std::chrono::steady_clock::time_point end;

    public:
        std::string server_url;
        struct sockaddr_in* server_addr;
        int port;
        std::string request_header;
        int sock_fd;
        struct pollfd *poll_fd = nullptr;
        uint8_t *response_buffer;
        int buffer_size;

        void set_server(const char*);
        void send_http_request();


    };

}
#endif //BASIC_HTTP_CLIENT_HTTPCLIENT_H
