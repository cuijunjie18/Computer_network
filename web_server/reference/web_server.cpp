#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <filesystem>

namespace fs = std::filesystem;

const int BUFFER_SIZE = 1024;
const int PORT = 6789;

// 发送文件内容
void send_file(int socket, const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("File not found");
    }

    // 读取文件内容
    std::string content((std::istreambuf_iterator<char>(file)), 
                        std::istreambuf_iterator<char>());

    // 构建HTTP响应
    std::string response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: " + std::to_string(content.size()) + "\r\n"
        "\r\n" + content;

    if (send(socket, response.c_str(), response.size(), 0) < 0) {
        throw std::runtime_error("Failed to send file");
    }
}

// 发送404错误
void send_404(int socket) {
    std::string response = 
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "<html><body><h1>404 Not Found</h1></body></html>";

    if (send(socket, response.c_str(), response.size(), 0) < 0) {
        std::cerr << "Failed to send 404 response" << std::endl;
    }
}

// 解析HTTP请求获取文件名
std::string parse_filename(const char* request) {
    std::string req_str(request);
    size_t start = req_str.find(' ') + 1;
    size_t end = req_str.find(' ', start);
    std::string path = req_str.substr(start, end - start);
    
    // 移除路径开头的'/'
    if (!path.empty() && path[0] == '/') {
        path = path.substr(1);
    }
    
    // 默认返回index.html
    if (path.empty()) {
        path = "index.html";
    }
    
    return path;
}

int main() {
    // 创建socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return 1;
    }

    // 设置socket选项
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        std::cerr << "Setsockopt failed" << std::endl;
        return 1;
    }

    // 绑定地址和端口
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return 1;
    }

    // 开始监听
    if (listen(server_socket, 1) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return 1;
    }

    std::cout << "Server ready on port " << PORT << std::endl;

    while (true) {
        std::cout << "Ready to serve..." << std::endl;

        // 接受客户端连接
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        
        if (client_socket < 0) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }

        // 接收HTTP请求
        char buffer[BUFFER_SIZE] = {0};
        ssize_t bytes_read = recv(client_socket, buffer, BUFFER_SIZE, 0);
        
        if (bytes_read < 0) {
            std::cerr << "Receive failed" << std::endl;
            close(client_socket);
            continue;
        }

        try {
            // 解析文件名
            std::string filename = parse_filename(buffer);
            
            // 发送文件
            send_file(client_socket, filename);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            send_404(client_socket);
        }

        close(client_socket);
    }

    close(server_socket);
    return 0;
}