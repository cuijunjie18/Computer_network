#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"  // 服务器 IP（本地测试）
#define PORT 8001               // 服务器端口

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr;

    // 1. 创建 UDP Socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 2. 配置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;          // IPv4
    server_addr.sin_port = htons(PORT);        // 服务器端口
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid server IP");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // 3. 发送消息到服务器
    const char *message = "Hello from UDP Client!";
    sendto(sockfd, message, strlen(message), 0,
          (const struct sockaddr *)&server_addr, sizeof(server_addr));
    std::cout << "Message sent to server: " << message << std::endl;

    // 4. 接收服务器响应
    int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, nullptr, nullptr);
    if (n < 0) {
        perror("Recvfrom failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    buffer[n] = '\0';  // 确保字符串终止
    std::cout << "Server response: " << buffer << std::endl;

    // 5. 关闭 Socket
    close(sockfd);
    return 0;
}