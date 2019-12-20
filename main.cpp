#include <iostream>
#include "basic_http_client.h"
#include "poll.h"


int main() {

    std::cout << "Hello, World!" << std::endl;
    auto* req = new basic_http_client::HttpClient;
    struct pollfd _poll = {0};
    //req->poll_fd = &_poll;
    req->send_http_request();
    return 0;
}
