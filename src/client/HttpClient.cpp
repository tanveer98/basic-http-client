//
// Created by tanveer on 20.12.19.
//

#include <iostream>
#include <fcntl.h>
#include <cstring>
#include <netdb.h>
#include "HttpClient.h"
#include <openssl/err.h>
#include <tls.h>
#include "interface-io/IO_Functions.h"

#define HTTP_PORT 80
#define  API_IP "37.139.1.159"


basic_http_client::HttpClient::HttpClient() {
    this->bufferSize_ = BUFSIZ;
    this->responseBuffer_ = static_cast<uint8_t *>(calloc(bufferSize_, sizeof(*responseBuffer_)));
    this->serverAddr_ = static_cast<sockaddr_in *>(calloc(1, sizeof(*serverAddr_)));
}

basic_http_client::HttpClient::HttpClient(const char *domain_name, int port) : HttpClient() {
    using namespace basic_http_client;
    this->port_ = port;
    this->domain_name_ = domain_name;
    if (this->port_ == HTTPS) {
        struct tls_config *cfg = nullptr;
        int res = 0;
        const char *ca_path = "/etc/ssl/certs/";

        tls_init();
        this->ctx_ = tls_client();
        cfg = tls_config_new();
        res = tls_config_set_ca_path(cfg, ca_path);
        res = tls_configure(ctx_, cfg);
        if (res == -1) {
            //clean up
            std::cout << "TLS Configuration failed, aborting";
            tls_close(ctx_);
            tls_config_free(cfg);
            tls_free(ctx_);
            exit(2);
        }
    }
};

basic_http_client::HttpClient::~HttpClient() {
    this->bufferSize_ = 0;
    if (this->responseBuffer_ != nullptr) { free(this->responseBuffer_); }
    if (this->serverAddr_ != nullptr) { free(this->serverAddr_); }
    if (this->ctx_ != nullptr) { tls_free(ctx_); }
    if (this->pollFd_ != nullptr) { free(this->pollFd_); }
}

/**
* prepares an existing socket for async IO operation
 * @return 0 on success, -1 on failiure
*/
int basic_http_client::HttpClient::async_socket() {
    if (!this->isAsync_) { return 0; }
    this->pollFd_ = static_cast<pollfd *>(calloc(1, sizeof(*pollFd_)));
    int status = fcntl(this->sockFd_, F_SETFL, fcntl(this->sockFd_, F_GETFL, 0) | O_NONBLOCK);
    if (status == -1) { return -1; }

    this->pollFd_->fd = sockFd_;
    this->pollFd_->events = (POLLIN | POLLOUT);
    poll(this->pollFd_, 1, 0);
    return 0;
}

/***
 * creates a socket (with a random port) and makes it async!;
 * @return sockfd;
 */
int basic_http_client::HttpClient::create_client_socket() {

    this->sockFd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    std::cout << "Created socket" << std::endl;
    return this->sockFd_;
}

/**
 *
 * @param sock_fd
 * @return return 0 on success, -1 on failure
 */
int basic_http_client::HttpClient::connect_server() {
    int res = 0;
    auto conn = [&]() {
        return connect(this->sockFd_, (struct sockaddr *) this->serverAddr_, sizeof *serverAddr_);
    };

    res = conn();
    async_socket();
    return res;
}

/**
 *
 * @param sock_fd
 * @return number of bytes sent on success
 */

int basic_http_client::HttpClient::send_request() {
    int sent_bytes = IO_Functions::Send(this);
    std::cout << "Sent bytes: " << sent_bytes <<
              std::endl;
    return sent_bytes;
}

/**
 *
 * recv response from a socket, stores it in  this.responseBuffer_ returns a pointer to the buffer.
 * @return buffer
 */

int basic_http_client::HttpClient::recv_response() {
    int recvd_bytes = IO_Functions::Recv(this);
    std::cout << "Recieved bytes: " << recvd_bytes <<
              std::endl;
    return recvd_bytes;
}

void basic_http_client::HttpClient::set_server(const char *domain_name) {
    using namespace basic_http_client;
    struct addrinfo *res = resolve_ip(domain_name);
    //copy the 4byte ip addr.
    struct sockaddr_in *tmp = (struct sockaddr_in *) res->ai_addr;
    this->serverAddr_->sin_addr.s_addr = tmp->sin_addr.s_addr;
    this->serverAddr_->sin_port = htons(this->port_);
    this->serverAddr_->sin_family = AF_INET;
    freeaddrinfo(res);
}

int basic_http_client::HttpClient::create_tls() {
    std::string request_ = this->request_header->req_header_;
    int res = 0;

    res = tls_connect_socket(this->ctx_, this->sockFd_, this->domain_name_.c_str());
    if (res < 0) {
        std::cout << "\n\n\n\nCreate tls failed, exiting" << std::endl;
        exit(102);
    };
    //send
    //send_request();
    //recv
    //recv_response();
    return 0;
}

void basic_http_client::HttpClient::send_http_request() {
    using namespace basic_http_client;
    this->begin = std::chrono::steady_clock::now();
    char *json_body = nullptr;
    //resolve ip address from server_url/domain name
    set_server(this->domain_name_.c_str());
    //create socket
    create_client_socket();
    //connect to server
    if (connect_server() < 0) throw "Connection failed";
    std::cout << "connected socket" << std::endl;
    if (this->port_ == HTTPS) {
        create_tls();
    }
    send_request();
    recv_response();

    //parse json body
    json_body = (char *) (this->responseBuffer_);
    std::cout << json_body << std::endl;
    json_body = strstr(json_body, "\r\n\r\n");
    json_body = strchr(json_body, '{');

    std::cout << json_body << std::endl;
    this->end = std::chrono::steady_clock::now();
    std::cout << "Time difference = "
              << std::chrono::duration_cast<std::chrono::milliseconds>(this->end - this->begin).count()
              << "[ms]" << std::endl;


}


struct addrinfo *basic_http_client::resolve_ip(const char *domain_name) {
    struct addrinfo *res;
    getaddrinfo(domain_name, NULL, NULL, &res);
    return res;
}

void basic_http_client::show_ip(const char *domain_name) {
    const struct addrinfo *orig = resolve_ip(domain_name);
    struct addrinfo *tmp = const_cast<addrinfo *>(orig);

    while (tmp != nullptr) {
        struct sockaddr_in *addr = (struct sockaddr_in *) tmp->ai_addr;
        struct in_addr ip = addr->sin_addr;
        std::string ip_addr_str = inet_ntoa(ip);
        std::cout << ip_addr_str << " and AI_PROTOCOL " << tmp->ai_protocol << " AND socket type " << tmp->ai_socktype
                  << std::endl;
        tmp = tmp->ai_next;
    }


}


