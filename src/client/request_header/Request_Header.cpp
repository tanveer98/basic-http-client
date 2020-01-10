//
// Created by tanveer on 10.01.20.
//

#include <boost/format.hpp>
#include <cstring>
#include "Request_Header.h"

basic_http_client::Request_Header::Request_Header(basic_http_client::REQ_METHOD mthd) {
    using basic_http_client::HttpClient;
    this->method = mthd;
    if (this->method == basic_http_client::GET) {
        this->req_header = boost::format("GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nAccept-Encoding: deflate\r\nConnection: close\r\n\r\n");
    } else if (this->method == basic_http_client::POST) {
        this->req_header = boost::format("POST %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nContent-Length: %d\r\nAccept-Encoding: deflate\r\nConnection: Close\r\nContent-Type: application/json\r\n\r\n%s");
    }
}

void basic_http_client::Request_Header::format_request(const char *domain, const char *uri) {
    if (this->method == basic_http_client::GET) {
        this->req_header = (boost::format(this->req_header) % uri % domain);
    }
    this->req_header_ = this->req_header.str();
}

void basic_http_client::Request_Header::format_request(const char *domain, const char *uri, const char *payload) {
    size_t content_len = strlen(payload);
    if (this->method == basic_http_client::POST) {
        this->req_header = (boost::format(this->req_header) % uri % domain % content_len % payload);
    }
    this->req_header_ = this->req_header.str();
}
