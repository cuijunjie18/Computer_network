#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXN 2048

void parse_url(const char *url,std::string &server_ip,
                std::string &server_port,
                std::string &filePath)
{
    int p;
    for (p = 0; url[p] != ':' && url[p] != '/'; p++) server_ip += url[p];
    if (url[p] == ':'){
        p++;
        for (; url[p] != '/'; p++) server_port += url[p];
        p++;
    }else{
        server_port = "80"; // 未指定端口使用默认端口.
    }
    for (; url[p] != '\0'; p++) filePath += url[p];
    if (filePath.empty()) filePath = "/"; // 默认根目录
}

std::string Get_request(
                const std::string &server_ip,
                const std::string &server_port,
                const std::string &filePath)
{
    std::string request  =  "GET /" + filePath + " HTTP/1.1\r\n"
                            "Host: " + server_ip + ":" + server_port;
    return request;
}

int main(int argc,char *argv[])
{
    if (argc < 2){
        std::cerr << "usage:" << argv[0] << " <url>" << std::endl;
        return -1;
    }

    // Parse url
    std::string server_ip;
    std::string server_port;
    std::string filePath;
    parse_url(argv[1], server_ip, server_port, filePath);
    uint16_t port = atoi(&server_port[0]);

    // Generate socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0){
        std::cerr << "Generate socket failed!" << std::endl;
        return -1;
    }

    // Set web_server addr
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET,&server_ip[0],&server_addr.sin_addr) < 0){
        std::cerr << "Server ip is invalid!" << std::endl;
        return -1;
    }

    // Connect to web_server
    if (connect(client_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
        std::cerr << "Connect failed!" << std::endl;
        return -1;
    }
    std::cout << "Connect to " << server_ip << std::endl; 

    // 发送请求
    char recv_buf[MAXN];
    std::string send_buf = Get_request(server_ip, server_port, filePath);

    while (true){
        if (send(client_socket, &send_buf[0], send_buf.size(), 0) < 0){
            std::cerr << "Send failed!" << std::endl;
            break;
        }
        int recv_length = recv(client_socket, recv_buf, MAXN, 0);
        recv_buf[recv_length] = '\0';
        printf("%s\n",recv_buf);
        break;
    }

    close(client_socket);
    return 0;
}