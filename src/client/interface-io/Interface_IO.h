//
// Created by tanveer on 19.01.20.
//

#ifndef BASIC_HTTP_CLIENT_INTERFACE_IO_H
#define BASIC_HTTP_CLIENT_INTERFACE_IO_H

#include "../../../basic_http_client.h"
#include "../https-client/HttpsClient.h"
#include "../HttpClient.h"

namespace basic_http_client {
    class Interface_IO {
    private:
        typedef int (*Send_Fn_Ptr)(HttpClient *, int total_sent);

        typedef int (*Recv_Fn_Ptr)(HttpClient *, int total_recvd);


        static int send_(HttpClient *client, int sent_bytes) {
            std::string header = client->request_header->req_header_;
            int to_be_sent = header.size();
            int x = 0;
            (client->port_ == HTTP) ?
                    x = send(client->sockFd_, (header.c_str() + sent_bytes), to_be_sent, 0)
                                    : x = tls_write(client->ctx_, (header.c_str() + sent_bytes), to_be_sent);
            return x;
        };

        static int recv_(HttpClient *client, int total_recvd) {
            uint8_t *buff = client->responseBuffer_;
            int buff_size = client->bufferSize_;
            int recvd = 0;
            //instead off BUFSIZ, make it buff_size - total_recvd?
            (client->port_ == HTTP) ?
                    recvd = recv(client->sockFd_, buff + total_recvd, BUFSIZ, 0)
                                    : recvd = tls_read(client->ctx_, buff + total_recvd, BUFSIZ);
            return recvd;
        };

        static int send_async(HttpClient *client, int total_sent) {
            int res = 0;
            int sent = 0;
            res = poll(client->pollFd_, 1, 0);
            if ((res != 0) && ((client->pollFd_->revents & POLLOUT))) {
                sent = send_(client, total_sent);
            } else {
                sent = WAIT_FOR_DATA;
            }
            return sent;
        };

        static int recv_async(HttpClient *client, int total_recvd) {
            int res = 0;
            int recvd = 0;
            res = poll(client->pollFd_, 1, 0);
            if ((res > 0) && (client->pollFd_->revents & POLLIN)) {
                recvd = recv_(client, total_recvd);
            } else {
                recvd = WAIT_FOR_DATA;
            }
            return recvd;
        };
    public:
       static const int WAIT_FOR_DATA = -101;
        static int Send(HttpClient *client) {
            Send_Fn_Ptr send_fn_ptr = (client->isAsync_) ? send_async : send_;
            std::string request_ = client->request_header->req_header_;
            const char *header = request_.c_str();
            int to_be_sent = request_.size();
            int sent_bytes = 0;
            int res = 0;
            while (sent_bytes < to_be_sent) {
                res = send_fn_ptr(client, sent_bytes);
                if (res == -1) { exit(101); }
                // this if is necessary for TLS socket,
                // otherwise, sent_bytes is decremented by 2, which means we might send too much data
                // when the socket gets readable, better to just continue without touching anything.
                if (res == TLS_WANT_POLLIN || res == WAIT_FOR_DATA) { continue; }
                sent_bytes += res;
            }

            return sent_bytes;
        };

        static int Recv(HttpClient *client) {
            Recv_Fn_Ptr recv_fn_ptr = (client->isAsync_) ? recv_async :  recv_;
            uint8_t *buff = client->responseBuffer_;
            int buff_size = client->bufferSize_;
            int total_recvd = 0;
            while (true) {
                int recv_bytes = 0;
                recv_bytes = recv_fn_ptr(client, total_recvd);
                if (recv_bytes == TLS_WANT_POLLOUT || recv_bytes == WAIT_FOR_DATA) { continue; }
                if (recv_bytes <= 0) { break; }

                total_recvd += recv_bytes;
                if (total_recvd > (buff_size - 50)) {
                    buff_size += BUFSIZ;
                    buff = (uint8_t *) realloc(buff, buff_size);
                }
            }
            if (buff_size > total_recvd) {
                buff_size = total_recvd;
                client->bufferSize_ = buff_size;
            }
            buff = (uint8_t *) realloc(buff, buff_size);
            client->responseBuffer_ = buff;
            return total_recvd;
        };
    };
}

#endif //BASIC_HTTP_CLIENT_INTERFACE_IO_H
