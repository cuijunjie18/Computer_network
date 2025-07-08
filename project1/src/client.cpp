#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
// #include <netinet/in.h>

#define BUFFER_SIZE 2048

std::string get_send_message(std::string &client_name,std::string &num){
    std::cout << "Input your name:";
    std::cin >> client_name;
    std::cout << "Input a number:";
    std::cin >> num;
    return client_name + "\n" + num; // 以换行为信息切分点
}

void parse_recv(char *recv_buffer,int bytes_received,
    std::string &server_name,std::string &server_num){
    int p = 0;
    for (p = 0; p < bytes_received; p++){
        if (recv_buffer[p] == '\n') break;
        server_name += recv_buffer[p];
    }
    p++;
    for (; p < bytes_received; p++){
        server_num += recv_buffer[p];
    }
    return;
}

int main(int argc,char *argv[])
{
    if (argc < 3){
        printf("Usage:%s <server_ip> <server_port>\n",argv[0]);
        return -1; 
    }

    // 解析参数
    const char* server_ip = argv[1];
    uint16_t port = atoi(argv[2]);

    // 创建socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1){
        std::cerr << "Socket creation failed." << std::endl;
        return -1;
    }

    // 设置服务器地址结构体
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET,server_ip,&server_addr.sin_addr) <= 0){
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }

    // 连接服务器
    if (connect(client_socket,(struct sockaddr*) &server_addr, sizeof(server_addr)) < 0){
        std::cerr << "Connection Failed" << std::endl;
        return -1;
    }

    std::cout << "Connected to server at " << server_ip << ":" << port << std::endl;

    char recv_buffer[BUFFER_SIZE];
    std::string client_name = "";
    std::string client_num = "";
    std::string server_name = "";
    std::string server_num = "";
    int sum = 0;

    while (true){
        // 发
        std::string send_message = get_send_message(client_name,client_num);
        if (send(client_socket,&send_message[0],send_message.size(),0) < 0){
            std::cerr << "Send failed" << std::endl;
            break;
        }

        // 收
        int bytes_received = recv(client_socket, recv_buffer, BUFFER_SIZE - 1,0);
        if (bytes_received > 0) {
            parse_recv(recv_buffer,bytes_received,server_name,server_num);
            std::cout << "local name: " << client_name << ", select num: " << client_num << "\n";
            std::cout << "server name: " << server_name << ", select num: " << server_num << "\n";
            sum = atoi(&client_num[0]) + atoi(&server_num[0]);
            std::cout << "Sum: " << sum << "\n";
        } else if (bytes_received == 0) {
            std::cout << "Server disconnected." << std::endl;
        } else {
            std::cerr << "Receive failed" << std::endl;
        }
        break;
    }

    // 关闭socket描述符
    close(client_socket);

    return 0;
}