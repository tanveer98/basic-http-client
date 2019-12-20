#include <iostream>
#include "http_client.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    basic_http_client::send_http_request();
    return 0;
}
