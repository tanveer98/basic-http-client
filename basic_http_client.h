//
// Created by tanveer on 19.12.19.
//

#ifndef BASIC_HTTP_CLIENT_BASIC_HTTP_CLIENT_H
#define BASIC_HTTP_CLIENT_BASIC_HTTP_CLIENT_H

#include <netdb.h>

namespace basic_http_client
{
    enum Protocol : int {
        HTTP = 80, HTTPS = 443
    };
    enum REQ_METHOD : int {
        GET, POST
    };

    struct addrinfo *resolve_ip(const char *domain_name);
    void show_ip(const char* domain_name);

    class HttpClient;
    class Cmdline;
    class Request_Header;
    class Interface_IO;
}

#endif //BASIC_HTTP_CLIENT_BASIC_HTTP_CLIENT_H
