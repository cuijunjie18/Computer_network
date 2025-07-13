#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#define MAXN 1024

// parse the domain name to sockaddr
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
        inet_ntop(p->ai_family, sockp, ip, p->ai_addrlen);
        break;
    }
    return sockp->sin_addr;
}

// send email
int send_email(int &client_socket,char *recv_buf){
    int bytes_recv = 0;

    // send host mail_info
    std::string sender_mail = "MAIL FROM:<test@example.com>\r\n";
    if (send(client_socket, &sender_mail[0], sender_mail.size(), 0) < 0){
        std::cerr << "Send host mail_info failed!" << std::endl;
        return -1;
    }
    std::cout << "Send host mail_info successfully!" << std::endl;
    bytes_recv = recv(client_socket, recv_buf, MAXN, 0);
    if (bytes_recv <= 0){
        std::cerr << "Received failed!" << std::endl;
        return -1;
    }
    std::string response_1(recv_buf);
    std::cout << response_1 << std::endl;
    std::cout << "==========================" << std::endl;
    if (response_1.substr(0,3) != "250"){
        std::cerr << "No 250 response received from server!" << std::endl;
        return -1;
    }

    // send receiver mail_info
    std::string receiver_mail = "2871843852@qq.com";
    if (send(client_socket, &receiver_mail[0], receiver_mail.size(), 0) < 0){
        std::cerr << "Send receiver mail_info failed!" << std::endl;
        return -1;
    }
    bytes_recv = recv(client_socket, recv_buf, MAXN, 0);
    if (bytes_recv <= 0){
        std::cerr << "Received failed!" << std::endl;
        return -1;
    }
    std::string response_2(recv_buf);
    if (response_2.substr(0,3) != "250"){
        std::cerr << "No 250 response received from server!" << std::endl;
        return -1;
    }
    std::cout << response_2 << std::endl;
    std::cout << "==========================" << std::endl;

    // 
    return 1;
}

int main(int argc,char *argv[])
{
    if (argc < 3){
        std::cout << "usage: " << argv[0] << " <server_ip>/<server_web> <server_port>" << std::endl;
        return -1;
    }

    // Generate tcp socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0){
        std::cerr << "Generate socket failed!" << std::endl;
        return -1;
    }

    // Set server ip
    sockaddr_in server_addr;
    uint16_t server_port = std::atoi(argv[2]);
    server_addr.sin_port = htons(server_port);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr = parse_domain_name(argv[1]);

    // connect to server
    if (connect(client_socket, (sockaddr *) &server_addr, sizeof(server_addr)) < 0){
        std::cerr << "Connect to server " << argv[1] << " failed!" << std::endl;
        close(client_socket);
        return -1;
    }
    std::cout << "Connect to server " << argv[1] << std::endl;

    // Set the range of outtime for client socket(5 s)
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    if (setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0){
        std::cerr << "Set socket opt failed!" << std::endl;
        close(client_socket);
        return -1;
    }

    // indentify server get connect request
    char recv_buf[MAXN];
    int bytes_recv = 0;
    bytes_recv = recv(client_socket, recv_buf, MAXN, 0);
    if (bytes_recv <= 0){
        std::cerr << "Received failed!" << std::endl;
        close(client_socket);
        return -1;
    }
    std::string response_1(recv_buf);
    if (response_1.substr(0,3) != "220"){
        std::cerr << "No 220 response received from server!" << std::endl;
        close(client_socket);
        return -1;
    }

    // send HELO instruction to server
    std::string send_HELO = "HELO cuijunjie18\r\n";
    if (send(client_socket, &send_HELO[0], send_HELO.size(), 0) < 0){
        std::cerr << "Send HELO failed!" << std::endl;
        close(client_socket);
        return -1;
    }
    bytes_recv = recv(client_socket, recv_buf, MAXN, 0);
    if (bytes_recv <= 0){
        std::cerr << "Received failed!" << std::endl;
        close(client_socket);
        return -1;
    }
    std::string response_2(recv_buf);
    if (response_2.substr(0,3) != "250"){
        std::cerr << "No 250 response received from server!" << std::endl;
        close(client_socket);
        return -1;
    }

    // send email to server
    std::cout << "Connect with mail application successfully!Wait to send e-mail..." << std::endl;
    if (send_email(client_socket,recv_buf) < 0){
        std::cerr << "Email send failed!" << std::endl;
        close(client_socket);
        return -1;
    }
    std::cout << "Email send successfully!" << std::endl;
    
    close(client_socket);
    return 0;
}