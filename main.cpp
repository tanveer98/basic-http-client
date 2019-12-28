#include <iostream>
#include <cstring>
#include "basic_http_client.h"
#include "poll.h"
#include <boost/format.hpp>


int main(int argc, char *argv[]) {
    using namespace basic_http_client;
    if (argc != 2) {
        puts("usage: ./basic-http-client <api domain name>");
        return 0;
    };

    std::cout << "Hello, World!" << std::endl;

    HttpClient openWeatherMap;
    int x = HttpClient::Protocol::HTTP;
    openWeatherMap.request_ = "GET /data/2.5/weather?q=Tallinn&appid=940e55b3552b342bfa536b74c819a4cc HTTP/1.1\r\nHost: api.openweathermap.org\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nAccept-Encoding: gzip, deflate\r\nConnection: close\r\n\r\n";
    openWeatherMap.serverUrl_ = argv[1];
    openWeatherMap.port_ = HttpClient::Protocol::HTTP;


    try {
        openWeatherMap.send_http_request();
        basic_http_client::show_ip(argv[1]);
    }
    catch (const char *e) {
        std::cerr << e << std::endl;
    }

/*    HttpClient random_org(argv[1],HttpClient::Protocol::HTTPS);
    boost::format request_header;
    std::string request_body;
    std::string request;
    int req_body_len = 0;

    request_body = R"({"jsonrpc":"2.0","method":"getUsage","params":{"apiKey":"00000000-0000-0000-0000-000000000000"},"id":625})";
    req_body_len = request_body.size();
    //POST request must contain a content-length (size of the payload)
    request_header = boost::format(
            "POST /json-rpc/2/invoke HTTP/1.1\r\nHost: %s\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nContent-Length: %d\r\nAccept-Encoding: deflate\r\nConnection: Keep-Alive\r\nContent-Type: application/json\r\n\r\n%s") %
                                  argv[1] % req_body_len % request_body;

    random_org.request_ = request_header.str();;
    random_org.isAsync_ = true;
    try {
        random_org.send_http_request();
        //basic_http_client::show_ip(argv[1]);
    }
    catch (const char *e) {
        std::cerr << e << std::endl;
    }*/
    return 0;
}
