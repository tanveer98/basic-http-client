//
// Created by tanveer on 10.01.20.
//

#ifndef BASIC_HTTP_CLIENT_REQUEST_HEADER_H
#define BASIC_HTTP_CLIENT_REQUEST_HEADER_H


#include "../../../basic_http_client.h"
#include "boost/format.hpp"

class basic_http_client::Request_Header {
public:
    REQ_METHOD method;
    boost::format req_header;
    std::string req_header_;

    void format_request(const char *domain, const char *uri);

    void format_request(const char *domain, const char *uri, const char *payload);

    explicit Request_Header(basic_http_client::REQ_METHOD m);
};


#endif //BASIC_HTTP_CLIENT_REQUEST_HEADER_H
