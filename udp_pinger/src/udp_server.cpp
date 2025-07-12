#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <random>
#include <time.h>
#include <unistd.h>
#include <stdio.h>

#define MAXN 2048
#define SERVER_PORT 8001

int main(int argc,char *argv[])
{
    // set the seed
    std::srand(time(NULL));

    // Generate udp socket
    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket < 0){
        std::cerr << "Generate socket failed!" << std::endl;
        return -1;
    }

    // bind socket to port
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        std::cerr << "Bind failed!" << std::endl;
        return -1;
    }
    std::cout << "UDP Server listening on port " << SERVER_PORT << std::endl;

    // client
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    char recv_buf[MAXN];
    char client_ip[64];

    while (true) {
        int recv_bytes = recvfrom(server_socket, recv_buf, MAXN, 0, 
            (struct sockaddr *) &client_addr, &client_len);
        bool if_recv = (rand() % 10) > 3; // 模拟40%的丢包率
        if (!if_recv) continue;

        if (recv_bytes){
            inet_ntop(AF_INET,&(client_addr.sin_addr),client_ip,client_len);
        }else if (recv_bytes < 0){
            printf("Connect close.\n");
            break;
        }

        recv_buf[recv_bytes] = '\0';
        printf("Recv %d bytes from %s\n",recv_bytes,client_ip);
        if (sendto(server_socket,recv_buf,recv_bytes,0,
            (struct sockaddr *) &client_addr,client_len) < 0){
            std::cerr << "Send failed!" << std::endl;
            break;
        }
        std::cout << "Return data successfully!" << std::endl; 
    }

    close(server_socket);
    return 0;
}