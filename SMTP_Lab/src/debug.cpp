#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>
#include <string.h>

int main(int argc,char *argv[])
{
    if (argc < 2){
        std::cerr << "usage: " << argv[0] << " <domain_name>" << std::endl;
        return -1; 
    }

    addrinfo *listp,*p,hints;
    sockaddr_in *sockp;
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(argv[1], NULL, &hints, &listp) != 0){
        std::cerr << "Get addrinfo error!" << std::endl;
    }

    char ip[2048];
    for (p = listp; p != nullptr; p = p->ai_next){
        sockp = (sockaddr_in*) p->ai_addr;
        inet_ntop(p->ai_family, &sockp->sin_addr, ip, p->ai_addrlen);
        std::cout << ip << std::endl;
    }

    return 0;
}