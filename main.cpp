#include <iostream>
#include "poll.h"
#include <boost/format.hpp>
#include "basic_http_client.h"
#include "src/client/request_header/Request_Header.h"
#include "src/client/HttpClient.h"
#include "src/cmdline/Cmdline.h"

int main(int argc, char *argv[]) {
    using  basic_http_client::HttpClient;
    using basic_http_client::Cmdline;
    if (argc != 2) {
        puts("usage: ./basic-http-client <api domain name>");
        return 0;
    };

    auto *openWeatherMap  = new HttpClient();
    //openWeatherMap.request_header = "GET /data/2.5/weather?q=Tallinn&appid=940e55b3552b342bfa536b74c819a4cc HTTP/1.1\r\nHost: api.openweathermap.org\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nAccept-Encoding: deflate\r\nConnection: close\r\n\r\n";
    openWeatherMap->domain_name_ = argv[1];
    openWeatherMap->isAsync_ = true;
    openWeatherMap->port_ = basic_http_client::Protocol::HTTP;

    const char *arr[] = {"http://api.openweathermap.org/data/2.5/weather?q=Tallinn&appid=940e55b3552b342bfa536b74c819a4cc", "GET", "not async"};
    auto* cmd = new Cmdline(arr);
    try {
        cmd->client->send_http_request();
        basic_http_client::show_ip(argv[1]);
    }
    catch (const char *e) {
        std::cerr << e << std::endl;
    }




/*    HttpClient random_org("api.random.org", HttpClient::Protocol::HTTPS);
    struct pollfd _poll = {0};
    struct sockaddr_in s_addr_in;
    boost::format request_header;
    std::string request_body;
    std::string request;
    int req_body_len = 0;

    request_body = R"({"jsonrpc":"2.0","method":"getUsage","params":{"apiKey":"00000000-0000-0000-0000-000000000000"},"id":14437})";
    req_body_len = request_body.size();
    //POST request must contain a content-length (size of the payload)
    request_header = boost::format(
            "POST /json-rpc/2/invoke HTTP/1.1\r\nHost: %s\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nContent-Length: %d\r\nAccept-Encoding: deflate\r\nConnection: Close\r\nContent-Type: application/json\r\n\r\n%s") %
                                  argv[1] % req_body_len % request_body;

    request = request_header.str();
    std::cout << "\t*******request: " << request << " and size : " << request_body.size() << " ********" << std::endl;
    random_org.isAsync_ = false;
    random_org.request_ = request;
    try {
        random_org.send_http_request();
        basic_http_client::show_ip(argv[1]);
    }
    catch (const char *e) {
        std::cerr << e << std::endl;
    }*/
    return 0;
}
