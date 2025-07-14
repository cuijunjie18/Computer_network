#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

#define MAXN 1024

static const std::string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

// 解析域名
in_addr parse_domain_name(char *domain_name);

// 设置等待服务器响应超时时间
int set_range_outtime(int &client_socket,int seconds);

// 连接SMTP服务器
int connect_with_smtp(int &client_socket);

// 创建SSL上下文
SSL_CTX* createSSLContext();

// 加密连接
SSL* connect_with_ssl(int &client_socket);

// 加密发送
int send_email_ssl(SSL* ssl);

// base64 加密
std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);

# endif
