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
    if (argc == 2) {
        puts("usage: ./basic-http-client <api domain name>");
        return 0;
    };

    //const char *arr1[] = {"http://api.openweathermap.org/data/2.5/weather?q=Tallinn&appid=940e55b3552b342bfa536b74c819a4cc", "GET",};
   //const char *arr2[] = {"https://api.random.org/json-rpc/2/invoke" , "POST", R"({"jsonrpc":"2.0","method":"generateStrings","params":{"apiKey":"00000000-0000-0000-0000-000000000000","n":10,"length":4,"characters":"abcdefghijklmnopqrstuvwxyz","replacement":true},"id":5337})"};
    //const char *arr3[] = {"https://ics-calc-team7.tk/api/logs/response", "GET", "async"};
    auto* cmd = new Cmdline();
    try {
//        int arg_count = sizeof(arr1) / sizeof(*arr1);
//        cmd->send_http_request(arg_count, arr1);
//        arg_count = sizeof(arr2) / sizeof(*arr2);
//        cmd->send_http_request(arg_count, arr2);
        //basic_http_client::show_ip("ics-calc-team7.tk");

        cmd->send_http_request(argc - 1, &argv[1]);
    }
    catch (const char *e) {
        std::cerr << e << std::endl;
    }

    return 0;
}
