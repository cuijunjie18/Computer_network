#ifndef PROXY_SERVER_HPP
#define PROXY_SERVER_HPP
#include "common.hpp"

#define MAXN 1024

class ProxyServer
{
    public:
        ProxyServer(int port);
        ~ProxyServer();
        void connect_with_client();
        void interact_with_client();
        void parse_http_request(char *buf);
        void send_back_file(std::string filePath);
        void debug_send_back_file(std::string filePath);
        void get_remote_file(std::string filePath,std::string save_path);
        void save_to_cache(char *body,std::string save_path);
        void free_resources();

        // 作为客户端的代理服务器变量
        int listen_socket;
        int connect_socket;
        int proxy_server_port;
        sockaddr_in proxy_server_addr;

        // 作为请求客户端的变量
        int client_socket;
        sockaddr_in remote_server_addr;

    private:
        const std::string save_prefix = "saves/";
        char recv_buf[MAXN];

        // 隐藏客户端信息
        sockaddr_in client_addr;
        socklen_t client_len;
        char client_buf[MAXN];
        std::string domain_name = ""; // 客户请求的域名
        std::string filePath = ""; // 客户需要的文件
};

#endif