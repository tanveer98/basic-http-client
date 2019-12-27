//
// Created by tanveer on 20.12.19.
//

#include <iostream>
#include <fcntl.h>
#include <cstring>
#include <netdb.h>
#include "HttpClient.h"
#include "basic_http_client.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#define HTTP_PORT 80
#define  API_IP "37.139.1.159"

/***
 * create socket
 * @return sock_fd > 0 on success, -1 on failure
 */
namespace basic_http_client {
    int HttpClient::create_client_socket() {
        struct sockaddr_in client_addr = {0};
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(0);
        client_addr.sin_addr.s_addr = INADDR_ANY;

        this->sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (bind(sock_fd, (struct sockaddr *) &client_addr, sizeof client_addr) < 0) throw "bind failed";
        std::cout << "Created socket" << std::endl;
        return this->sock_fd;
    }

/**
 *
 * @param sock_fd
 * @return return 0 on success, -1 on failure
 */

    int HttpClient::connect_server() {
        if (this->sock_fd <= 0) return -1;
        return connect(this->sock_fd, (struct sockaddr *) this->server_addr, sizeof *server_addr);
    }

/**
 *
 * @param sock_fd
 * @return number of bytes sent on success
 */

    int HttpClient::send_request() {

        const char *header = this->request_header.c_str();
        int to_be_sent = request_header.size();
        int sent_bytes = 0;

        while (sent_bytes < to_be_sent) {
            to_be_sent -= sent_bytes;
            sent_bytes += send(this->sock_fd, header + sent_bytes, to_be_sent, 0);
        }
        std::cout << "Sent bytes: " << sent_bytes << std::endl;
        this->begin = std::chrono::steady_clock::now();
        return sent_bytes;
    }

/**
 *
 * @param sock_fd
 * @param poll_fd
 * @return 0 on success, -1 on failiure
 */

    int HttpClient::async_socket() {
        if (this->poll_fd == nullptr) return 0;

        int status = fcntl(this->sock_fd, F_SETFL, fcntl(this->sock_fd, F_GETFL, 0) | O_NONBLOCK);
        if (status == -1) return -1;

        this->poll_fd->fd = sock_fd;
        this->poll_fd->events = POLLIN;
        poll(this->poll_fd, 1, 10000);

        return 0;
    }

/**
 *
 * @param sock_fd
 * @param poll_fd
 * @param buffer
 * @param buffer_size
 * @return buffer
 */

    uint8_t *HttpClient::recv_response() {
        uint8_t *buff = this->response_buffer;
        int buff_size = this->buffer_size;
        int total = 0;

        while (true) {
            int recv_bytes = 0;
            if ((poll_fd != nullptr)) {
                //non-blocking
                int res = poll(poll_fd, 1, 0);
                std::cout << res << std::endl;
                if ((res > 0)) {
                    recv_bytes = recv(sock_fd, buff + total, BUFSIZ, 0);
                    this->end = std::chrono::steady_clock::now();
                    std::cout << "Time difference = "
                              << std::chrono::duration_cast<std::chrono::milliseconds>(this->end - this->begin).count()
                              << "[µs]" << std::endl;
                    if (recv_bytes <= 0) { break; }
                } else {
                    std::cout << "No data yet, i could be doing something else here.... like counting from 1-10,000"
                              << std::endl;

                    for (int i = 0; i < 10000; i++) {
                        std::cout << i << std::endl;
                    }
                }
            } else {
                //blocking
                recv_bytes = recv(sock_fd, buff + total, BUFSIZ, 0);
                this->end = std::chrono::steady_clock::now();
                std::cout << "Time difference = "
                          << std::chrono::duration_cast<std::chrono::milliseconds>(this->end - this->begin).count()
                          << "[µs]" << std::endl;
                if (recv_bytes <= 0) { break; }
            }

            total += recv_bytes;
            //increment buffsize  by 1024bytes(BUFSIZ) when its almost full
            if (total > (buff_size - 50)) {
                buff_size += BUFSIZ;
                buff = (uint8_t *) realloc(buff, buff_size);

            }
        }

        if (buff_size > total) {
            buff_size = total;
            this->buffer_size = buff_size;
        }

        buff = (uint8_t *) realloc(buff, buff_size);
        this->response_buffer = buff;
        return buff;
    }

    void HttpClient::set_server(const char *domain_name) {
        // std::getline(std::cin,this->server_url);

        struct addrinfo *res = resolve_ip(domain_name);
        //copy the 4byte ip addr.
        struct sockaddr_in *tmp = (struct sockaddr_in *) res->ai_addr;
        this->server_addr->sin_addr.s_addr = tmp->sin_addr.s_addr;
        this->server_addr->sin_port = htons(this->port);
        this->server_addr->sin_family = AF_INET;
        freeaddrinfo(res);
    }

    int HttpClient::create_tls() {

        int ssl_sock = 0;
        int res = 0;
        const SSL_METHOD *method = nullptr;
        SSL_CTX *ctx = nullptr;
        SSL *ssl = nullptr;


        SSL_library_init();
        SSLeay_add_ssl_algorithms();
        SSL_load_error_strings();

        method = TLSv1_2_client_method();
        ctx = SSL_CTX_new(method);
        ssl = SSL_new(ctx);

        ssl_sock = SSL_get_fd(ssl);
        SSL_set_fd(ssl,this->sock_fd);
        res = SSL_connect(ssl);
        if( res <= 0) {
            std::cout << "Connect failed!";
            unsigned int err;
            while(err = ERR_get_error() ) {
                char* err_str = ERR_error_string(err,0);
                if(err_str != nullptr) {
                    puts(err_str);
                }
            }
        }
        //send
        const char *header = this->request_header.c_str();
        int to_be_sent = request_header.size();
        int sent_bytes = 0;
        std::cout << "The request header is :\n " << this->request_header << std::endl;
        while (sent_bytes < to_be_sent) {
            to_be_sent -= sent_bytes;

            sent_bytes += SSL_write(ssl, header , to_be_sent);
        }
        std::cout << "Sent bytes: " << sent_bytes << std::endl;
        this->begin = std::chrono::steady_clock::now();
        //send end;

        //recv
        int recv_bytes = 0;
        uint8_t * buff = this->response_buffer;
        memset(buff,0, this->buffer_size);
        int buff_size = this->buffer_size;
        int total = 0;

        //while(true) {
            recv_bytes = SSL_read(ssl, buff, BUFSIZ);
            //if(recv_bytes <= 0) { break; }

//            total += recv_bytes;
//            //increment buffsize  by 1024bytes(BUFSIZ) when its almost full
//            if (total > (buff_size - 50)) {
//                buff_size += BUFSIZ;
//                buff = (uint8_t *) realloc(buff, buff_size);p
//            }
        //}
        std::cout << (char*) buff << std::endl;
        //recv end


        return ssl_sock;
    }


    void HttpClient::send_http_request() {

        int buff_size = 0;
        uint8_t *buff = nullptr;
        int res = 0;
        char *json_body = nullptr;

        //resolve ip address from server_url/domain name
        set_server(this->server_url.c_str());
        //create socket
        res = create_client_socket();

        //connect to server
        if (connect_server() < 0) throw "Connection failed";
        std::cout << "connected socket" << std::endl;
        if (this->port == 443) {

            buff_size = BUFSIZ;
            buff = (uint8_t *) realloc(nullptr, buff_size);
            this->buffer_size = BUFSIZ;
            this->response_buffer = buff;

            create_tls();
        } else {

            //Send request
            send_request();
            if (async_socket() < 0) throw "Fcntl failed :(";

            //recv request
            buff_size = BUFSIZ;
            buff = (uint8_t *) realloc(nullptr, buff_size);
            this->buffer_size = BUFSIZ;
            this->response_buffer = buff;

            buff = recv_response();
        }

        //parse json body
        //char *json_body = strstr((char *) buff, "\r\n\r\n");
        //json_body = strchr((char *) buff, '{');
        json_body = (char *) (buff);
        std::cout << json_body << std::endl;

    }

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


