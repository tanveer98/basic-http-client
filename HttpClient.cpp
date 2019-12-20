//
// Created by tanveer on 20.12.19.
//

#include <iostream>
#include <fcntl.h>
#include <cstring>
#include "HttpClient.h"


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

        struct sockaddr_in server_addr = {0};
        server_addr.sin_addr.s_addr = inet_addr(API_IP);
        server_addr.sin_port = htons(HTTP_PORT);
        server_addr.sin_family = AF_INET;
        return connect(this->sock_fd, (struct sockaddr *) &server_addr, sizeof server_addr);
    }

/**
 *
 * @param sock_fd
 * @return number of bytes sent on success
 */

    int HttpClient::send_request() {
        std::string request_header = "GET /data/2.5/weather?q=Tallinn&appid=940e55b3552b342bfa536b74c819a4cc HTTP/1.1\r\nHost: api.openweathermap.org\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nAccept-Encoding: gzip, deflate\r\nConnection: close\r\n\r\n";
        const char *header = request_header.c_str();
        int to_be_sent = request_header.size();
        int sent_bytes = 0;

        while (sent_bytes < to_be_sent) {
            to_be_sent -= sent_bytes;
            sent_bytes += send(this->sock_fd, header + sent_bytes, to_be_sent, 0);
        }
        std::cout << "Sent bytes: " << sent_bytes << std::endl;

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
                if (poll_fd->revents && POLLIN) {
                    recv_bytes = recv(sock_fd, buff + total, BUFSIZ, 0);
                    if (recv_bytes <= 0) { break; }
                }
            } else {
                //blocking
                recv_bytes = recv(sock_fd, buff + total, BUFSIZ, 0);
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

    void HttpClient::send_http_request() {
        //create socket
        int sock_fd = create_client_socket();
        //connect to server
        if (connect_server() < 0) throw "Connection failed";
        std::cout << "connected socket" << std::endl;
        //Send request
        send_request();
        //async io
        //struct pollfd poll_fd{0};
        if (async_socket() < 0) throw "Fcntl failed :(";

        //recv request
        int buff_size = BUFSIZ;
        uint8_t *buff = (uint8_t *) realloc(nullptr, buff_size);
        this->buffer_size = BUFSIZ;
        this->response_buffer = buff;

        buff = recv_response();


        //parse json body
        char *json_body = strstr((char *) buff, "\r\n\r\n");
        json_body = strchr((char *) buff, '{');

        std::cout << json_body;
    }

}
