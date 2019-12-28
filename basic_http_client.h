//
// Created by tanveer on 19.12.19.
//

#ifndef BASIC_HTTP_CLIENT_BASIC_HTTP_CLIENT_H
#define BASIC_HTTP_CLIENT_BASIC_HTTP_CLIENT_H

#include "HttpClient.h"
#include <netdb.h>

namespace basic_http_client
{

    struct addrinfo *resolve_ip(const char *domain_name);
    void show_ip(const char* domain_name);
    class HttpClient;
}
#endif //BASIC_HTTP_CLIENT_BASIC_HTTP_CLIENT_H
