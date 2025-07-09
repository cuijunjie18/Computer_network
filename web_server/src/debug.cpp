#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <filesystem>

#define MAXN 2048

// 构造404相应报文
void response_404(char** response_buf){
    std::string response =  "HTTP/1.1 404 Not Found\r\n"
                            "Content-Type: text/html\r\n"
                            "\r\n"
                            "<html><body><h1>404 Not Found</h1></body></html>";
    *response_buf = &response[0];
    printf("%s\n",*response_buf);
}

int main(int argc,char *argv[])
{
    char *response = new char[MAXN];
    response_404(&response);
    printf("\n");
    printf("%s\n",response);
    return 0;
}