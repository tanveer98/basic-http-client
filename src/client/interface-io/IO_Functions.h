//
// Created by tanveer on 20.01.20.
//

#ifndef BASIC_HTTP_CLIENT_IO_FUNCTIONS_H
#define BASIC_HTTP_CLIENT_IO_FUNCTIONS_H

#include "../HttpClient.h"

namespace basic_http_client::IO_Functions {
    int Send(HttpClient *client);

    int Recv(HttpClient *client);
}
#endif //BASIC_HTTP_CLIENT_IO_FUNCTIONS_H
