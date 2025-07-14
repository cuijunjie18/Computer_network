#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

// Base64编码函数
std::string base64_encode(const std::string& in) {
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, in.c_str(), static_cast<int>(in.length()));
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    
    std::string result(bufferPtr->data, bufferPtr->length);
    BIO_free_all(bio);
    return result;
}

// 发送数据并接收响应
bool send_recv(int sock, const std::string& data, int expected_code, bool print_response = true) {
    if (send(sock, data.c_str(), data.length(), 0) == -1) {
        perror("发送失败");
        return false;
    }
    
    char buffer[1024];
    ssize_t bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) {
        perror("接收失败");
        return false;
    }
    
    buffer[bytes_received] = '\0';
    if (print_response) {
        std::cout << "服务器响应: " << buffer;
    }
    
    if (expected_code != 0) {
        if (atoi(buffer) != expected_code) {
            std::cerr << "错误: 期望响应码 " << expected_code << "，实际收到: " << buffer;
            return false;
        }
    }
    return true;
}

// SSL版本
bool send_recv(SSL* ssl, const std::string& data, int expected_code, bool print_response = true) {
    if (SSL_write(ssl, data.c_str(), static_cast<int>(data.length())) <= 0) {
        std::cerr << "SSL发送失败" << std::endl;
        return false;
    }
    
    char buffer[1024];
    int bytes_received = SSL_read(ssl, buffer, sizeof(buffer) - 1);
    if (bytes_received <= 0) {
        std::cerr << "SSL接收失败" << std::endl;
        return false;
    }
    
    buffer[bytes_received] = '\0';
    if (print_response) {
        std::cout << "服务器响应: " << buffer;
    }
    
    if (expected_code != 0) {
        if (atoi(buffer) != expected_code) {
            std::cerr << "错误: 期望响应码 " << expected_code << "，实际收到: " << buffer;
            return false;
        }
    }
    return true;
}

int main() {
    // 初始化OpenSSL
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    // 邮箱配置
    const std::string sender = "287852@qq.com";
    const std::string password = "exmogdcjd"; // 实际使用需替换为真实授权码
    const std::string receiver = "2293366@qq.com";
    const std::string server = "smtp.qq.com";
    const int port = 587; // 使用STARTTLS的端口

    // 创建socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("创建socket失败");
        return 1;
    }

    // 解析服务器地址
    struct hostent* host = gethostbyname(server.c_str());
    if (!host) {
        std::cerr << "无法解析主机名" << std::endl;
        close(sock);
        return 1;
    }

    struct sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr = *((struct in_addr*)host->h_addr);

    // 连接服务器
    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr))) {
        perror("连接服务器失败");
        close(sock);
        return 1;
    }
    std::cout << "已连接到SMTP服务器" << std::endl;

    // 接收初始响应
    if (!send_recv(sock, "", 220)) {
        close(sock);
        return 1;
    }

    // 发送EHLO
    if (!send_recv(sock, "EHLO Alice\r\n", 250)) {
        close(sock);
        return 1;
    }

    // 发送STARTTLS
    if (!send_recv(sock, "STARTTLS\r\n", 220)) {
        close(sock);
        return 1;
    }

    // 创建SSL上下文
    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        std::cerr << "创建SSL上下文失败" << std::endl;
        close(sock);
        return 1;
    }

    // 创建SSL对象
    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);

    // 建立SSL连接
    if (SSL_connect(ssl) != 1) {
        std::cerr << "SSL握手失败" << std::endl;
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(sock);
        return 1;
    }
    std::cout << "已建立SSL连接" << std::endl;

    // 在SSL连接上重新发送EHLO
    if (!send_recv(ssl, "EHLO Alice\r\n", 250)) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(sock);
        return 1;
    }

    // 认证过程
    if (!send_recv(ssl, "AUTH LOGIN\r\n", 334)) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(sock);
        return 1;
    }

    // 发送Base64编码的用户名
    if (!send_recv(ssl, base64_encode(sender) + "\r\n", 334)) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(sock);
        return 1;
    }

    // 发送Base64编码的密码
    if (!send_recv(ssl, base64_encode(password) + "\r\n", 235)) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(sock);
        return 1;
    }

    // 设置发件人
    if (!send_recv(ssl, "MAIL FROM: <" + sender + ">\r\n", 250)) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(sock);
        return 1;
    }

    // 设置收件人
    if (!send_recv(ssl, "RCPT TO: <" + receiver + ">\r\n", 250)) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(sock);
        return 1;
    }

    // 准备发送数据
    if (!send_recv(ssl, "DATA\r\n", 354)) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(sock);
        return 1;
    }

    // 构造邮件内容
    std::string msg = "From: \"Linux测试\" <" + sender + ">\r\n"
                      "To: \"收件人姓名\" <" + receiver + ">\r\n"
                      "Subject: Linux C++ SMTP测试\r\n"
                      "Content-Type: text/plain; charset=\"utf-8\"\r\n\r\n"
                      "这是一封来自Linux C++的测试邮件\r\n"
                      "使用OpenSSL和Linux socket实现\r\n"
                      "邮件发送功能测试成功！\r\n.\r\n";

    // 发送邮件内容
    if (!send_recv(ssl, msg, 250)) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(sock);
        return 1;
    }

    // 退出
    if (!send_recv(ssl, "QUIT\r\n", 221)) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(sock);
        return 1;
    }

    // 清理资源
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    close(sock);

    std::cout << "邮件发送成功！" << std::endl;
    return 0;
}