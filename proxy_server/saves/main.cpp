#include "common.hpp"
#include "utils.hpp"
#include "proxy_server.hpp"

char buf[MAXN];

int main(int argc,char *argv[])
{
    // Generate client socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0){
        std::cerr << "Generate socket error!" << std::endl;
        return -1;
    }

    // Set server
    std::string domain_name = "baidu.com";
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80); // http的默认端口
    server_addr.sin_addr = parse_domain_name(&domain_name[0]);
    inet_ntop(AF_INET, &server_addr.sin_addr, buf, sizeof(server_addr));
    std::string ip(buf);
    std::cout << ip <<std::endl;

    // connect to server
    if (connect(client_socket, (sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        std::cerr << "Fail connect to server!" << std::endl;
        close(client_socket);
        return -1;
    }

    // Make http request
    std::string filePath = "";
    std::string http_request = 
            "GET /" + filePath + " HTTP/1.1\r\n"
            "Host: " + domain_name + "\r\n"
            "\r\n";
    if (filePath.empty()) filePath = "index.html";

    // Send
    std::cout << http_request << std::endl;
    if (send(client_socket, &http_request[0], http_request.size(), 0) < 0){
        std::cerr << "Send failed!" << std::endl;
        close(client_socket);
        return -1;
    }

    // Get response
    char *body = nullptr;
    int bytes = recv(client_socket, buf, MAXN, 0);
    if (bytes > 0){
        int p = get_body(buf,bytes);
        if (p == -1){
            std::cerr << "Invalid format of response!" << std::endl;

        }
        body = &buf[p + 4];
        // save_to_cache(body,filePath);
    }else{
        std::cout << "Fail recv!" << std::endl;
        close(client_socket);
        return -1;
    }
    
    close(client_socket);
    return 0;
}