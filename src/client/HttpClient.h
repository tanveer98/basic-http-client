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
#include <tls.h>
#include <string>
#include <functional>
#include "../../basic_http_client.h"
#include "request_header/Request_Header.h"

class basic_http_client::HttpClient {
    typedef int (*Recv_fn_ptr)(HttpClient *this_, uint8_t *buffer, int recvd);
    typedef int (*Send_fn_ptr)(HttpClient *this_, int to_be_sent, int sent_bytes, const char *header);
private:
    int create_client_socket();

    int connect_server();

    int send_request(Send_fn_ptr);

    int async_socket();

    int create_ssl();

//    uint8_t *recv_response(const std::function<int(HttpClient *, uint8_t *, int)> &);
    uint8_t *recv_response(Recv_fn_ptr);

    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;

public:
    bool isAsync_ = false;
    int port_{};
    int bufferSize_{};
    int sockFd_{};
    uint8_t *responseBuffer_{nullptr};
    Request_Header *request_header;
    std::string domain_name_;
    struct sockaddr_in *serverAddr_ = nullptr;
    struct pollfd *pollFd_ = nullptr;
    struct tls *ctx_ = nullptr;

    HttpClient();

    HttpClient(const char *domain_name, int port);

    ~HttpClient();

    int create_tls();

    void set_server(const char *);

    void send_http_request();

};


#endif //BASIC_HTTP_CLIENT_HTTPCLIENT_H
