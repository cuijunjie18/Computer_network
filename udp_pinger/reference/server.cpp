#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define PORT 8080

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // 1. 创建 UDP Socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 2. 绑定 IP 和端口
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // 监听所有网卡
    server_addr.sin_port = htons(PORT); // 端口

    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    std::cout << "UDP Server listening on port " << PORT << std::endl;

    while (true) {
        // 3. 接收数据
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                        (struct sockaddr *)&client_addr, &client_len);
        if (n < 0) {
            perror("Recvfrom failed");
            continue;
        }
        buffer[n] = '\0'; // 确保字符串终止

        // 打印客户端信息
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        std::cout << "Received from " << client_ip << ": " << buffer << std::endl;

        // 4. 发送响应
        const char *response = "Message received!";
        sendto(sockfd, response, strlen(response), 0,
              (const struct sockaddr *)&client_addr, client_len);
    }

    // 5. 关闭 Socket（通常不会执行到这里）
    close(sockfd);
    return 0;
}