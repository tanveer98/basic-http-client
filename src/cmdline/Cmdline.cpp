//
// Created by tanveer on 10.01.20.
//

#include "Cmdline.h"
#include <string>
#include <iostream>
#include <cstring>
#include "../client/HttpClient.h"
#include "../client/request_header/Request_Header.h"
#include "Cmdline.h"

void basic_http_client::Cmdline::send_http_request(int argc, char **argv) {
    using std::string;
    using std::endl;
    using std::cerr;

    string url_str{argv[0]};
    size_t pos1;
    size_t pos2;
    string start, end;
    char c;

    pos1 = url_str.find("://");
    c = url_str[pos1 - 1];
    (c == 's' || c == 'S') ? this->proto = basic_http_client::HTTPS : this->proto = basic_http_client::HTTP;
    pos1 += 3;
    pos2 = url_str.find('/', pos1);
    start = url_str.substr(pos1, (pos2 - pos1));
    end = url_str.substr(pos2);

    this->domain_name = start.c_str();
    this->query = end.c_str();
    this->method = (strcmp(argv[1], "POST") == 0) ? basic_http_client::POST : basic_http_client::GET;
    if((this->method == POST && argc == 3) || (this->method == GET && argc == 2)){ this->isAsync = false;}
    else { this->isAsync = (strcmp(argv[2], "async") == 0); }

    this->client = new HttpClient(this->domain_name, this->proto);
    this->client->isAsync_ = this->isAsync;
    this->client->request_header = new Request_Header(this->method);
    if (this->method == basic_http_client::GET) {
        this->client->request_header->format_request(this->domain_name, this->query);
    } else if (this->method == basic_http_client::POST) {
        this->client->request_header->format_request(this->domain_name, this->query, argv[argc -1]);
    }

    cerr << this->proto << this->query << endl;
    this->client->send_http_request();
}
