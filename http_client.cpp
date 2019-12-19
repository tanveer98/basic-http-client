#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstdlib>
#include <iostream>
#include <fcntl.h>
#include <poll.h>
#include <cstdio>
#include <unistd.h>

#define HTTP_PORT 80
#define  API_IP "37.139.1.159"

void send_request() {
    //create socket
    struct sockaddr_in client_addr = {0};
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(0);
    client_addr.sin_addr.s_addr = INADDR_ANY;

    int sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (bind(sock_fd, (struct sockaddr *) &client_addr, sizeof client_addr) < 0)
        throw "bind failed";
    std::cout << "Created socket" <<std::endl;
    //

    //connect to server

    struct sockaddr_in server_addr = {0};
    server_addr.sin_addr.s_addr = inet_addr(API_IP);
    server_addr.sin_port = htons(HTTP_PORT);
    server_addr.sin_family = AF_INET;

    if (connect(sock_fd, (struct sockaddr *) &server_addr, sizeof server_addr) < 0)
        throw "Connection failed";
    std::cout << "connected socket" <<std::endl;

    //Send request
    std::string request_header = "GET /data/2.5/weather?q=Tallinn&appid=940e55b3552b342bfa536b74c819a4cc HTTP/1.1\r\nHost: api.openweathermap.org\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nAccept-Encoding: gzip, deflate\r\n\r\n";
    const char* header = request_header.c_str();
    int to_be_sent = request_header.size();
    int sent_bytes = 0;

    while (sent_bytes < to_be_sent)
    {
        to_be_sent -= sent_bytes;
        sent_bytes += send(sock_fd,header+sent_bytes,to_be_sent,0);
    }
    std::cout << "Sent bytes: " << sent_bytes << std::endl;

    //async io
    int status = fcntl(sock_fd, F_SETFL, fcntl(sock_fd, F_GETFL, 0) | O_NONBLOCK);
    if (status == -1) throw "Fcntl failed :(";
    struct pollfd _poll;
    _poll.fd = sock_fd;
    _poll.events = POLLIN;
    poll(&_poll,1,10000);
    //

    //recv request
    int recv_bytes = 0;
    int total = 0;
    int buff_size = BUFSIZ;
    char* buff = (char*) realloc(nullptr,buff_size);

    while(true)
    {
        if(_poll.revents && POLLIN) {
            recv_bytes = recv(sock_fd, buff + total, BUFSIZ, 0);
            if (recv_bytes <= 0)
                break;

            total += recv_bytes;
            if (total > (buff_size - 50)) {
                buff_size += BUFSIZ;
                buff = (char *) realloc(buff, buff_size);
            }
        }
    }

    if(buff_size > total)
        buff_size = total;
    buff = (char*)realloc(buff,buff_size);
    std::cout<< "*********recieved bytes ***********" << recv_bytes << "total bytes " << total << std::endl;
    std::cout << buff << std::endl;


    FILE* fp = fopen("test.json", "w");
    int file_writen = fwrite(buff,1,buff_size,fp);
    std::cout << file_writen << " Bytes written to file!";
    fclose(fp);

    char cwd[255] = {0};
    getcwd(cwd,255);
    std::cout << cwd << std::endl;

}