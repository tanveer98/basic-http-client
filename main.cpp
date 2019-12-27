#include <iostream>
#include <cstring>
#include "basic_http_client.h"
#include "poll.h"
#include <boost/format.hpp>


int main(int argc, char *argv[]) {
    if (argc != 2) {
        puts("usage: ./basic-http-client <api domain name>");
        return 0;
    };

    std::cout << "Hello, World!" << std::endl;

//    basic_http_client::HttpClient openWeatherMap { };
//    struct pollfd _poll = {0};
//    openWeatherMap.poll_fd = &_poll;
//    openWeatherMap.request_header = "GET /data/2.5/weather?q=Tallinn&appid=940e55b3552b342bfa536b74c819a4cc HTTP/1.1\r\nHost: api.openweathermap.org\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nAccept-Encoding: gzip, deflate\r\nConnection: close\r\n\r\n";
//    openWeatherMap.server_url = argv[1];
//      openWeatherMap.port = 80;
//    struct  sockaddr_in s_addr;
//    openWeatherMap.server_addr = &s_addr;


    basic_http_client::HttpClient random_org{};
    struct pollfd _poll = {0};
    struct sockaddr_in s_addr_in;
    boost::format request_header;
    std::string request_body;
    std::string request;
    int req_body_len = 0;

    request_body = "{\"jsonrpc\":\"2.0\",\"method\":\"generateIntegers\",\"params\":{\"apiKey\":\"eb47436d-c9b3-4e12-bbcf-3de58d660dbc\",\"n\":1,\"min\":1,\"max\":10,\"replacement\":true,\"base\":10},\"id\":9840}";
    req_body_len = request_body.size();
    //POST request must contain a content-length (size of the payload)
    request_header = boost::format(
            "POST /json-rpc/2/invoke HTTP/1.1\r\nHost: %s\r\nAccept: */*\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nContent-Length: %d\r\nAccept-Encoding: deflate\r\nConnection: Keep-Alive\r\nContent-Type: application/json\r\n\r\n%s") %
                                  argv[1] % req_body_len % request_body;

    request = request_header.str();
    std::cout << "\t*******request: " << request << " and size : " << request_body.size() << " ********" << std::endl;

    random_org.poll_fd = &_poll;
    random_org.server_url = argv[1];
    random_org.server_addr = &s_addr_in;
    random_org.port = 443;
    random_org.request_header = request;
    // random_org.request_header = "POST /json-rpc/2/invoke HTTP/1.1\r\nHost: api.random.org\r\nAccept: */*\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nContent-Length: 106\r\nAccept-Encoding: deflate\r\nConnection: Keep-Alive\r\nContent-Type: application/json\r\n\r\n{\"jsonrpc\":\"2.0\",\"method\":\"getUsage\",\"params\":{\"apiKey\":\"eb47436d-c9b3-4e12-bbcf-3de58d660dbc\"},\"id\":9840};";
    try {
        random_org.send_http_request();
        basic_http_client::show_ip(argv[1]);
    }
    catch (const char *e) {
        std::cerr << e << std::endl;
    }
    return 0;
}
