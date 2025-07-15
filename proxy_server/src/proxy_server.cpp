#include "proxy_server.hpp"
#include "utils.hpp"

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
}

void ProxyServer::connect_with_client(){
    client_len = sizeof(client_addr);
    connect_socket = accept(listen_socket, (sockaddr*) &client_addr, &client_len);
    if (connect_socket < 0){
        std::cerr << "Connect failed!" << std::endl;
        free_resources();
        exit(1);
    }
    inet_ntop(AF_INET, &client_addr.sin_addr, client_buf, client_len);
    std::cout << "Connect with " << client_buf << std::endl; 
}

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
    if (send(connect_socket, header.c_str(), header.size(), 0) <= 0) {
        std::cerr << "Failed to send header" << std::endl;
        return;
    }

    // 5. 分块发送文件内容
    const size_t BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];
    while (!file.eof()) {
        file.read(buffer, BUFFER_SIZE);
        int bytes_read = file.gcount();
        if (send(connect_socket, buffer, bytes_read, 0) <= 0) {
            std::cerr << "Failed to send file content" << std::endl;
            return;
        }
    }

    return;
}

void ProxyServer::send_back_file(std::string filePath){
    char body[MAXN];

    // Open the file
    int fd = open(&filePath[0], O_RDONLY, 0);
    int filesize = read(fd,body,MAXN); // 以返回的字节数为filesize

    // Make http response
    std::string header = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: " + std::to_string(filesize) + "\r\n"
        "Connection: close\r\n\r\n";

    std::string send_body(body);
    std::string http_response = header + send_body;

    if (send(connect_socket, &http_response[0], http_response.size(), 0) < 0){
        std::cerr << "Send back file to client failed!" << std::endl;
        free_resources();
        exit(1);
    }
}

void get_remote_file(std::string filePath,std::string save_path){

}

void ProxyServer::interact_with_client(){
    while (true) {
        int bytes = recv(connect_socket, recv_buf, MAXN, 0); // MAXN确保一次接收完信息，其实不用while也ok的
        recv_buf[bytes] = '\0';
        if (bytes <= 0){
            std::cerr << "Connect with client close!" << std::endl;
            return;
        }
        parse_http_request(recv_buf);
        std::cout << "Request domain name: " << domain_name << std::endl;
        std::cout << "Request file: " << filePath << std::endl;
        std::cout << "Waiting for proxy..." << std::endl;
        std::string save_path = save_prefix + domain_name + "/" + filePath;
        std::cout << save_path << std::endl;
        if (file_is_exist(save_path)){
            send_back_file(save_path);
            // debug_send_back_file(save_path);
        }else{
            std::filesystem::create_directories(save_prefix + domain_name);

        }
    }
}

void ProxyServer::free_resources(){
    if (listen_socket >= 0) close(listen_socket);
    if (connect_socket >= 0) close(connect_socket);
}

ProxyServer::~ProxyServer(){
    if (listen_socket >= 0) close(listen_socket);
    if (connect_socket >= 0) close(connect_socket);
    std::cout << "Proxy_server close!" << std::endl;
}