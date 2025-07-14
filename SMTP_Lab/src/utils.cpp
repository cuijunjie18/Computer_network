#include "utils.h"

// Base64 encode
std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];
   
    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
   
            for(i = 0; (i <4) ; i++) ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }
   
    if (i)
    {
        for(j = i; j < 3; j++) char_array_3[j] = '\0';
   
        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;
   
        for (j = 0; (j < i + 1); j++) ret += base64_chars[char_array_4[j]];
   
        while((i++ < 3)) ret += '=';
    }
   
    return ret;
}  

// Parse the domain name to sockaddr
in_addr parse_domain_name(char *domain_name){
    addrinfo *listp,*p,hints;
    sockaddr_in *sockp;

    // init addrinfo
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(domain_name, NULL, &hints, &listp)){
        std::cerr << "Get addrinfo failed!" << std::endl;
        exit(1);
    }

    // 取第一个ip地址
    char ip[1024];
    for (p = listp; p != nullptr; p = p->ai_next){
        sockp = (sockaddr_in *) p->ai_addr;
        inet_ntop(p->ai_family, sockp, ip, p->ai_addrlen);
        break;
    }
    return sockp->sin_addr;
}

// Set the range of outtime when receive SMTP server's response
int set_range_outtime(int &client_socket,int seconds){
    struct timeval tv;
    tv.tv_sec = seconds;
    tv.tv_usec = 0;
    return setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

// Connect with SMTP server
int connect_with_smtp(int &client_socket){
    // indentify server get connect request
    char recv_buf[MAXN];
    int bytes_recv = 0;
    bytes_recv = recv(client_socket, recv_buf, MAXN, 0);
    if (bytes_recv <= 0){
        std::cerr << "Received failed!" << std::endl;
        return -1;
    }
    std::string response_1(recv_buf);
    if (response_1.substr(0,3) != "220"){
        std::cerr << "No 220 response received from server!" << std::endl;
        return -1;
    }

    // send HELO instruction to server
    std::string send_HELO = "HELO cuijunjie18\r\n";
    if (send(client_socket, &send_HELO[0], send_HELO.size(), 0) < 0){
        std::cerr << "Send HELO failed!" << std::endl;
        return -1;
    }
    bytes_recv = recv(client_socket, recv_buf, MAXN, 0);
    if (bytes_recv <= 0){
        std::cerr << "Received failed!" << std::endl;
        return -1;
    }
    std::string response_2(recv_buf);
    if (response_2.substr(0,3) != "250"){
        std::cerr << "No 250 response received from server!" << std::endl;
        return -1;
    }
    return 1;
}

// 创建 SSL 上下文
SSL_CTX* createSSLContext() {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    
    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        std::cerr << "Unable to create SSL context" << std::endl;
        ERR_print_errors_fp(stderr);
        return nullptr;
    }
    return ctx;
}

// Connect with ssl
SSL* connect_with_ssl(int &client_socket){
    char recv_buf[MAXN];
    int bytes_recv = 0;
    std::string response;

    // Start TLS
    std::string send_start = "starttls\r\n";
    if (send(client_socket, &send_start[0], send_start.size(), 0) < 0){
        std::cerr << "Send starttls failed!" << std::endl;
        return nullptr;
    }
    bytes_recv = recv(client_socket, recv_buf, MAXN, 0);
    if (bytes_recv <= 0){
        std::cerr << "Received failed!" << std::endl;
        return nullptr;
    }
    response = recv_buf;
    if (response.substr(0,3) != "220"){
        std::cerr << "No 220 response received from server!" << std::endl;
        return nullptr;
    }

    // 创建 SSL 上下文
    SSL_CTX* ctx = createSSLContext();
    if (!ctx) {
        return nullptr;
    }

    // 创建 SSL 结构
    SSL* ssl = SSL_new(ctx);
    if (!ssl) {
        std::cerr << "Unable to create SSL structure" << std::endl;
        SSL_CTX_free(ctx);
        return nullptr;
    }

    // 将 SSL 与 client_socketet 关联
    if (SSL_set_fd(ssl, client_socket) != 1) {
        std::cerr << "SSL_set_fd failed" << std::endl;
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        return nullptr;
    }

    // 建立 SSL 连接
    if (SSL_connect(ssl) != 1) {
        std::cerr << "SSL_connect failed" << std::endl;
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        return nullptr;
    }
    return ssl;
}

// Send email using ssl(Example qq.mail)
int send_email_ssl(SSL* ssl){
    char recv_buf[MAXN];
    int bytes_recv = 0;
    std::string response;
    std::string sender = "Your e-mail address";
    std::string passwd = "Your passwd";
    std::string receiver = "receiver e-mail address";

    // auth login
    std::string auth_message = "auth login\r\n";
    if (SSL_write(ssl, &auth_message[0], auth_message.size()) < 0){
        std::cerr << "Send AUTH LOGIN failed!" << std::endl;
        return -1;
    }
    bytes_recv = SSL_read(ssl, recv_buf, MAXN);
    if (bytes_recv <= 0){
        std::cerr << "Received failed!" << std::endl;
        return -1;
    }
    response = recv_buf;
    if (response.substr(0,3) != "334"){
        std::cerr << "No 334 response received from server!" << std::endl;
        return -1;
    }

    // authenticate your email
    std::string base64_sender = base64_encode((unsigned char *)&sender[0], sender.size());
    base64_sender += "\r\n";
    if (SSL_write(ssl, &base64_sender[0], base64_sender.size()) < 0){
        std::cerr << "Send sender failed!" << std::endl;
        return -1;
    }
    bytes_recv = SSL_read(ssl, recv_buf, MAXN);
    if (bytes_recv <= 0){
        std::cerr << "Received failed!" << std::endl;
        return -1;
    }
    response = recv_buf;
    if (response.substr(0,3) != "334"){
        std::cerr << "No 334 response received from server!" << std::endl;
        return -1;
    }

    // Send passwd
    std::string base64_passwd = base64_encode((unsigned char*)&passwd[0], passwd.size());
    base64_passwd += "\r\n";
    if (SSL_write(ssl, &base64_passwd[0], base64_passwd.size()) < 0){
        std::cerr << "Send passwd failed!" << std::endl;
        return -1;
    }
    bytes_recv = SSL_read(ssl, recv_buf, MAXN);
    if (bytes_recv <= 0){
        std::cerr << "Received failed!" << std::endl;
        return -1;
    }
    response = recv_buf;
    if (response.substr(0,3) != "235"){
        std::cerr << "No 235 response received from server!" << std::endl;
        return -1;
    }

    // send host mail_info
    std::string sender_mail = "MAIL FROM:<2871843852@qq.com>\r\n";
    if (SSL_write(ssl, &sender_mail[0], sender_mail.size()) < 0){
        std::cerr << "Send sender_mail failed!" << std::endl;
        return -1;
    }
    bytes_recv = SSL_read(ssl, recv_buf, MAXN);
    if (bytes_recv <= 0){
        std::cerr << "Received failed!" << std::endl;
        return -1;
    }
    response = recv_buf;
    if (response.substr(0,3) != "250"){
        std::cerr << "No 250 response received from server!" << std::endl;
        return -1;
    }

    // send receiver mail_info
    std::string receiver_mail = "RCPT TO:<2293393766@qq.com>\r\n";
    if (SSL_write(ssl, &receiver_mail[0], receiver_mail.size()) < 0){
        std::cerr << "Send receiver_mail failed!" << std::endl;
        return -1;
    }
    bytes_recv = SSL_read(ssl, recv_buf, MAXN);
    if (bytes_recv <= 0){
        std::cerr << "Received failed!" << std::endl;
        return -1;
    }
    response = recv_buf;
    if (response.substr(0,3) != "250"){
        std::cerr << "No 250 response received from server!" << std::endl;
        return -1;
    }

    // Send Data flag
    std::string Data_flag = "DATA\r\n";
    if (SSL_write(ssl, &Data_flag[0], Data_flag.size()) < 0){
        std::cerr << "Send Data_flag failed!" << std::endl;
        return -1;
    }
    bytes_recv = SSL_read(ssl, recv_buf, MAXN);
    if (bytes_recv <= 0){
        std::cerr << "Received failed!" << std::endl;
        return -1;
    }
    response = recv_buf;
    if (response.substr(0,3) != "354"){
        std::cerr << "No 354 response received from server!" << std::endl;
        return -1;
    }

    // Send the content of mail
    std::string msg = 
    "From:  \"Little_CJJ\" <" + sender + ">\r\n"
    "To: \"Future CJJ\" <" + receiver + ">\r\n"
    "Subject: Linux C++ SMTP、SSL测试\r\n"
    "Content-Type: text/plain; charset=\"utf-8\"\r\n\r\n"
    "这是一封来自Linux C++的测试邮件\r\n"
    "使用OpenSSL和Linux socket实现\r\n"
    "邮件发送功能测试成功！\r\n.\r\n";  // 其中 .为结束符
    if (SSL_write(ssl, &msg[0], msg.size()) < 0){
        std::cerr << "Send msg failed!" << std::endl;
        return -1;
    }
    bytes_recv = SSL_read(ssl, recv_buf, MAXN);
    if (bytes_recv <= 0){
        std::cerr << "Received failed!" << std::endl;
        return -1;
    }
    response = recv_buf;
    if (response.substr(0,3) != "250"){
        std::cerr << "No 250 response received from server!" << std::endl;
        return -1;
    }

    // Send Quit flag
    std::string Quit_flag = "QUIT\r\n";
    if (SSL_write(ssl, &Quit_flag[0], Quit_flag.size()) < 0){
        std::cerr << "Send Quit_flag failed!" << std::endl;
        return -1;
    }
    bytes_recv = SSL_read(ssl, recv_buf, MAXN);
    if (bytes_recv <= 0){
        std::cerr << "Received failed!" << std::endl;
        return -1;
    }
    response = recv_buf;
    std::cout << response << std::endl;
    
    return 1;
}
