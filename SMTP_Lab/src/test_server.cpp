#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 8001
#define MAXN 1024

int main(int argc,char *argv[])
{
    // Generate socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0){
        std::cerr << "Generate socket failed!" << std::endl;
        return -1;
    }

    // Set the server
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    // Bind socket to port
    if (bind(server_socket,(sockaddr*) &server_addr,sizeof(server_addr)) < 0){
        std::cerr << "Bind failed!" << std::endl;
        close(server_socket);
        return -1;
    }

    // Listen
    if (listen(server_socket, 1) < 0){
        std::cerr << "Listen socket failed!" << std::endl;
        close(server_socket);
        return -1;
    }
    std::cout << "Listen on port: " << SERVER_PORT << std::endl; 

    // Get client addr and generate tcp socket
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr); // 要初始化
    int tcp_socket = accept(server_socket, (sockaddr*) &client_addr, &client_len);
    if (tcp_socket < 0){
        std::cerr << "Accept error!" << std::endl;
        close(server_socket);
        return -1;
    }

    char ip[MAXN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), ip, client_len);
    std::cout << "Connect to ip: " << ip << std::endl;
    
    close(server_socket);
    close(tcp_socket);
    return 0;
}