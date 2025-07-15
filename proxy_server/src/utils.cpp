#include "utils.hpp"

// Parse the domain name to sockaddr
in_addr parse_domain_name(char *domain_name){
    addrinfo *listp,*p,hints;
    sockaddr_in *sockp;

    // init addrinfo
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(domain_name, NULL, &hints, &listp)){
        std::cerr << "Get addrinfo failed!" << std::endl;
        exit(1);
    }

    // 取第一个ip地址
    char ip[1024];
    for (p = listp; p != nullptr; p = p->ai_next){
        sockp = (sockaddr_in *) p->ai_addr;
        break;
    }
    return sockp->sin_addr;
}

// 字符串匹配 "\r\n\r\n"，复杂度o(4n)
int get_body(char *buf,int len){
    for (int i = 0; i + 3 < len; i++){
        if (buf[i] != '\r' || buf[i + 1] != '\n' || \
            buf[i + 2] != '\r' || buf[i + 3] != '\n') continue;
        return i;
    }
    return -1;
}

// Set the range of outtime when receive SMTP server's response
int set_range_outtime(int &client_socket,int seconds){
    struct timeval tv;
    tv.tv_sec = seconds;
    tv.tv_usec = 0;
    return setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

// Check whether file exist
bool file_is_exist(std::string filePath){
    int fd = open(&filePath[0], O_RDONLY, 0);
    bool flag = fd > 0;
    close(fd);
    return flag;
}