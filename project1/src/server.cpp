#include <string.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 2048

void parse_recv(char *recv_buffer,int bytes_received,
    std::string &client_name,std::string &client_num){
        int p = 0;
        for (p = 0; p < bytes_received; p++){
            if (recv_buffer[p] == '\n') break;
            client_name += recv_buffer[p];
        }
        p++;
        for (; p < bytes_received; p++){
            client_num += recv_buffer[p];
        }
        return;
}

int main(int argc,char *argv[])
{
    if (argc < 2){
        printf("Usage:%s <port>\n",argv[0]);
        exit(1);
    }

    // string to port
    uint16_t port = atoi(argv[1]);

    // 创建socket
    int server_socket = socket(AF_INET,SOCK_STREAM,0);
    if (server_socket == -1){
        std::cerr << "Socket creation failed." << std::endl;
        return -1;
    }

    // 设置服务器地址结构体
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // 本地任意
    server_addr.sin_port = htons(port);

    // 绑定socket
    if (bind(server_socket,(struct sockaddr*) &server_addr,sizeof(server_addr)) < 0){
        std::cerr << "Bind failed." << std::endl;
        return -1;
    }

    // 开始监听端口(最多同时处理1个请求)
    if (listen(server_socket, 1) < 0){
        std::cerr << "Listen failed." << std::endl;
        return -1;
    }
    std::cout << "Server is listening on port " << port << std::endl;

    // 接受客户端连接
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int connect_socket = accept(server_socket,(struct sockaddr*) &client_addr,&client_len);
    if (connect_socket < 0) {
        std::cerr << "Accept failed." << std::endl;
        return -1;
    }

    std::cout << "Client connected." << std::endl;

    char recv_buffer[BUFFER_SIZE];
    char send_buffer[BUFFER_SIZE];
    std::string client_name = "";
    std::string client_num = "";
    std::string server_name = "Three body";
    std::string server_num = "";

    while (true) {
        // 收
        int bytes_received = recv(connect_socket, recv_buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received > 0) {
            parse_recv(recv_buffer, bytes_received, client_name, client_num);
            int check_num = atoi(&client_num[0]);
            if (check_num > 100 || check_num < 1){
                std::cerr << "The number recevied from client is out of range!" << "\n";
                break;
            }
            std::cout << "client name: " << client_name << ",select: " << client_num << "\n";
            std::cout << "Input your num:";
            std::cin >> server_num;
            std::cout << "server name: " << server_name << ", select num: " << server_num << "\n";
            std::cout << "Sum:" << atoi(&client_num[0]) + atoi(&server_num[0]) << "\n";
        } else if (bytes_received == 0) {
            std::cout << "Client disconnected." << std::endl;
            break;
        } else {
            std::cerr << "Receive failed." << std::endl;
            break;
        }

        // 发
        std::string recv_message = server_name + "\n" + server_num;
        if (send(connect_socket,&recv_message[0],recv_message.size(), 0) < 0){
            std::cerr << "Send failed." << std::endl;
            break;
        }
        break;
    }

    close(connect_socket);
    close(server_socket);
    return 0;
}