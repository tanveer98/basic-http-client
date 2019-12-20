#include <iostream>
#include "basic_http_client.h"
#include "poll.h"


int main(int argc, char* argv[]) {
    if(argc != 2)
    {
        puts("usage: ./basic-http-client <api domain name>");
        return 0;
    };

    std::cout << "Hello, World!" << std::endl;
    auto* req = new basic_http_client::HttpClient;
    struct pollfd _poll = {0};
    //req->poll_fd = &_poll;
    struct  sockaddr_in s_addr;
    req->server_addr = &s_addr;
    try{
        req->send_http_request(argv[1]);
    }
    catch(const char* e)
    {
        std::cerr << e <<std::endl;
    }
    return 0;
}
