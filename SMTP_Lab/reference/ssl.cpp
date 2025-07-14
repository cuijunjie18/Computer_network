#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

using namespace std;

// 创建 SSL 上下文
SSL_CTX* createSSLContext() {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    
    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        cerr << "Unable to create SSL context" << endl;
        ERR_print_errors_fp(stderr);
        return nullptr;
    }
    return ctx;
}

int main() {
    const char* host = "smtp.qq.com";
    const int port = 25;
    
    // 创建套接字
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket creation failed");
        return EXIT_FAILURE;
    }

    // 解析主机名
    struct hostent* server = gethostbyname(host);
    if (!server) {
        cerr << "Error: no such host" << endl;
        close(sock);
        return EXIT_FAILURE;
    }

    // 设置服务器地址
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port);

    // 连接服务器
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) {
        perror("connect failed");
        close(sock);
        return EXIT_FAILURE;
    }

    // 接收初始响应
    char buffer[1024] = {0};
    if (recv(sock, buffer, sizeof(buffer) - 1, 0) < 0) {
        perror("recv failed");
        close(sock);
        return EXIT_FAILURE;
    }
    cout << "Server: " << buffer;
    if (strncmp(buffer, "220", 3) != 0) {
        cerr << "Expected 220 from server" << endl;
        close(sock);
        return EXIT_FAILURE;
    }

    // 发送 EHLO 命令
    string ehlo = "EHLO Alice\r\n";
    if (send(sock, ehlo.c_str(), ehlo.size(), 0) < 0) {
        perror("send EHLO failed");
        close(sock);
        return EXIT_FAILURE;
    }
    memset(buffer, 0, sizeof(buffer));
    if (recv(sock, buffer, sizeof(buffer) - 1, 0) < 0) {
        perror("recv EHLO response failed");
        close(sock);
        return EXIT_FAILURE;
    }
    cout << "EHLO response: " << buffer;
    if (strncmp(buffer, "250", 3) != 0) {
        cerr << "Expected 250 response" << endl;
        close(sock);
        return EXIT_FAILURE;
    }

    // 发送 STARTTLS 命令
    string starttls = "STARTTLS\r\n";
    if (send(sock, starttls.c_str(), starttls.size(), 0) < 0) {
        perror("send STARTTLS failed");
        close(sock);
        return EXIT_FAILURE;
    }
    memset(buffer, 0, sizeof(buffer));
    if (recv(sock, buffer, sizeof(buffer) - 1, 0) < 0) {
        perror("recv STARTTLS response failed");
        close(sock);
        return EXIT_FAILURE;
    }
    cout << "STARTTLS response: " << buffer;
    if (strncmp(buffer, "220", 3) != 0) {
        cerr << "Expected 220 response for STARTTLS" << endl;
        close(sock);
        return EXIT_FAILURE;
    }

    // 创建 SSL 上下文
    SSL_CTX* ctx = createSSLContext();
    if (!ctx) {
        close(sock);
        return EXIT_FAILURE;
    }

    // 创建 SSL 结构
    SSL* ssl = SSL_new(ctx);
    if (!ssl) {
        cerr << "Unable to create SSL structure" << endl;
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        close(sock);
        return EXIT_FAILURE;
    }

    // 将 SSL 与 socket 关联
    if (SSL_set_fd(ssl, sock) != 1) {
        cerr << "SSL_set_fd failed" << endl;
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(sock);
        return EXIT_FAILURE;
    }

    // 建立 SSL 连接
    if (SSL_connect(ssl) != 1) {
        cerr << "SSL_connect failed" << endl;
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(sock);
        return EXIT_FAILURE;
    }

    cout << "SSL connection established successfully!" << endl;
    cout << "SSL protocol: " << SSL_get_version(ssl) << endl;
    cout << "Cipher: " << SSL_get_cipher(ssl) << endl;

    // 后续操作（认证、发送邮件等）使用 SSL_write 和 SSL_read
    // 例如：SSL_write(ssl, data.c_str(), data.size());
    //        SSL_read(ssl, buffer, sizeof(buffer));

    // 清理资源
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    close(sock);

    return 0;
}