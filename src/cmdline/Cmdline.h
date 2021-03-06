//
// Created by tanveer on 10.01.20.
//

#ifndef BASIC_HTTP_CLIENT_CMDLINE_H
#define BASIC_HTTP_CLIENT_CMDLINE_H

#include "../../basic_http_client.h"

class basic_http_client::Cmdline {
    const char *domain_name;

    const char *query;
    bool isAsync = false;
    basic_http_client::Protocol proto;
    basic_http_client::REQ_METHOD method;
    HttpClient *client{};
public:

    Cmdline() = default;
    void send_http_request(int argc, char *argv[]);
};


#endif //BASIC_HTTP_CLIENT_CMDLINE_H
