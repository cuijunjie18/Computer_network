#include "proxy_server.hpp"
#include "utils.hpp"

// 构造函数，启动监听端口
ProxyServer::ProxyServer(int port){
    // Generate socket
    listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket < 0){
        std::cerr << "Generate socket failed!" << std::endl;
        exit(1);
    }

    // set server
    proxy_server_addr.sin_family = AF_INET;
    proxy_server_addr.sin_port = htons(port);
    proxy_server_addr.sin_addr.s_addr = INADDR_ANY; // 本地任意

    // bind listen socket to port
    if (bind(listen_socket,(sockaddr *) &proxy_server_addr,sizeof(proxy_server_addr)) < 0){
        std::cerr << "Bind failed!" << std::endl;
        free_resources();
        exit(1);
    }

    if (listen(listen_socket,1) < 0){
        std::cerr << "Listen failed!" << std::endl;
        free_resources();
        exit(1);
    }
    std::cout << "Server listen on port " << port << std::endl;

    // 创建待与远程服务器连接的socket
    connect_socket_ws = socket(AF_INET, SOCK_STREAM, 0);
    if (connect_socket_ws < 0){
        std::cerr << "Generate socket failed!" << std::endl;
        free_resources();
        exit(1);
    }
}

// 与客户端连接
void ProxyServer::connect_with_client(){
    client_len = sizeof(client_addr);
    connect_socket_wc = accept(listen_socket, (sockaddr*) &client_addr, &client_len);
    if (connect_socket_wc < 0){
        std::cerr << "Connect failed!" << std::endl;
        free_resources();
        exit(1);
    }
    inet_ntop(AF_INET, &client_addr.sin_addr, client_buf, client_len);
    std::cout << "Connect with " << client_buf << std::endl; 
}

// 解析客户端的http请求报文
void ProxyServer::parse_http_request(char *buf){
    int length = strlen(buf);
    int p = 0;
    while (buf[p] != '/') p++;
    p++; // skip '/'
    while (buf[p] != '/' && buf[p] != ' ') domain_name += buf[p++];
    if (buf[p] == ' ') filePath = "index.html";
    else{
        p++;
        while (buf[p] != ' ') filePath += buf[p++];
    }
}

// 对比调试用的文件回传
void ProxyServer::debug_send_back_file(std::string filePath){
    // 1. 打开文件
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return;
    }

    // 2. 获取文件大小
    auto file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::cout << file_size << "\n";

    // 3. 构建HTTP响应头
    std::string header = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: " + std::to_string(file_size) + "\r\n"
        "Connection: close\r\n\r\n";

    // 4. 先发送响应头
    if (send(connect_socket_wc, header.c_str(), header.size(), 0) <= 0) {
        std::cerr << "Failed to send header" << std::endl;
        return;
    }

    // 5. 分块发送文件内容
    const size_t BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];
    while (!file.eof()) {
        file.read(buffer, BUFFER_SIZE);
        int bytes_read = file.gcount();
        if (send(connect_socket_wc, buffer, bytes_read, 0) <= 0) {
            std::cerr << "Failed to send file content" << std::endl;
            return;
        }
    }

    return;
}

// 请求的文件回传
void ProxyServer::send_back_file(std::string filePath){
    char body[MAXN];

    // Open the file
    int fd = open(&filePath[0], O_RDONLY, 0);
    if (fd < 0){
        std::cerr << "Open local file: " << filePath << " failed!" << std::endl;
        free_resources();
        exit(1); 
    }
    int filesize = read(fd,body,MAXN); // 以返回的字节数为filesize

    // Make http response
    std::string header = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: " + std::to_string(filesize) + "\r\n"
        "Connection: close\r\n\r\n";

    std::string send_body(body);
    std::string http_response = header + send_body;

    if (send(connect_socket_wc, &http_response[0], http_response.size(), 0) < 0){
        std::cerr << "Send back file to client failed!" << std::endl;
        free_resources();
        exit(1);
    }
    std::cout << "Send file back successfully!" << std::endl;
}

// 连接请求服务器
void ProxyServer::connect_with_server(std::string domain_name){
    remote_server_addr.sin_family = AF_INET;
    remote_server_addr.sin_port = htons(80); // 网页请求默认端口
    remote_server_addr.sin_addr = parse_domain_name(&domain_name[0]);

    // Build connect
    if (connect(connect_socket_ws, (sockaddr *) &remote_server_addr, 
            sizeof(remote_server_addr)) < 0)
    {
        std::cerr << "Failed to connect with remote server: " << domain_name << std::endl;
        free_resources();
        exit(1); 
    }
}

// 代理服务器请求远程服务器文件
char* ProxyServer::get_remote_file(std::string filePath)
{
    // Make http request
    if (filePath == "index.html") filePath = ""; // 默认请求index.html 清空，模拟实际的web输入
    std::string http_request = 
            "GET /" + filePath + " HTTP/1.1\r\n"
            "Host: " + domain_name + "\r\n"
            "\r\n";

    std::cout << http_request << std::endl;

    // std::cout << http_request << std::endl;
    if (send(connect_socket_ws, &http_request[0], http_request.size(), 0) < 0){
        std::cerr << "Send failed!" << std::endl;
        free_resources();
        exit(-1);
    }

    // Get response
    char *body = nullptr;
    int bytes = recv(connect_socket_ws, recv_buf_ws, MAXN, 0);
    recv_buf_ws[bytes] = '\0'; // 确保最后以'\0'结尾
    std::cout << recv_buf_ws << std::endl;
    if (bytes > 0){
        int p = get_body(recv_buf_ws,bytes);
        if (p == -1){
            std::cerr << "Invalid format of response!" << std::endl;
        }
        body = &recv_buf_ws[p + 4];
    }else{
        std::cerr << "Fail recv from remote server!" << std::endl;
        free_resources();
        exit(-1);
    }
    return body;
}

// 保存请求文件到cache
void ProxyServer::save_to_cache(char *body,std::string save_path){
    int fd = open(&save_path[0],O_WRONLY | O_CREAT | O_TRUNC, 0644); // 没有创建、有的话TRUNC覆盖
    if (fd < 0){
        std::cerr << "Fail to create file to cache!" << std::endl;
        free_resources();
        exit(1);
    }
    std::cout << "Save:" << strlen(body) << std::endl;
    if (write(fd, body, strlen(body)) < 0){
        std::cerr << "Save failed!" << std::endl;
    }else std::cout << "Save successfully!" << std::endl;
}

void ProxyServer::interact_with_client(){
    while (true) {
        int bytes = recv(connect_socket_wc, recv_buf_wc, MAXN, 0); // MAXN确保一次接收完信息，其实不用while也ok的
        recv_buf_wc[bytes] = '\0';
        if (bytes <= 0){
            std::cerr << "Connect with client close!" << std::endl;
            return;
        }
        parse_http_request(recv_buf_wc);
        std::cout << "Request domain name: " << domain_name << std::endl;
        std::cout << "Request file: " << filePath << std::endl;
        std::cout << "Waiting for proxy...\n" << std::endl;
        std::string save_path = save_prefix + domain_name + "/" + filePath;
        std::cout << "save path: " << save_path << std::endl;
        if (file_is_exist(save_path)){
            send_back_file(save_path);
            // debug_send_back_file(save_path);
        }else{
            // Connect to remote server and get file
            connect_with_server(domain_name);
            char *body = get_remote_file(filePath);

            // save file to cache
            std::filesystem::create_directories(save_prefix + domain_name);
            save_to_cache(body, save_path);
            send_back_file(save_path);
        }
    }
}

// 释放资源
void ProxyServer::free_resources(){
    if (listen_socket >= 0) close(listen_socket);
    if (connect_socket_wc >= 0) close(connect_socket_wc);
    if (connect_socket_ws >= 0) close(connect_socket_ws);
}

// 析构函数
ProxyServer::~ProxyServer(){
    if (listen_socket >= 0) close(listen_socket);
    if (connect_socket_wc >= 0) close(connect_socket_wc);
    if (connect_socket_ws >= 0) close(connect_socket_ws);
    std::cout << "Proxy_server close!" << std::endl;
}