#include "utils.h"

int main(int argc,char *argv[])
{
    if (argc < 3){
        std::cout << "usage: " << argv[0] << " <server_ip>/<server_web> <server_port>" << std::endl;
        return -1;
    }

    // Generate tcp socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0){
        std::cerr << "Generate socket failed!" << std::endl;
        return -1;
    }

    // Set server ip
    sockaddr_in server_addr;
    uint16_t server_port = std::atoi(argv[2]);
    server_addr.sin_port = htons(server_port);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr = parse_domain_name(argv[1]);

    // connect to server
    if (connect(client_socket, (sockaddr *) &server_addr, sizeof(server_addr)) < 0){
        std::cerr << "Connect to server " << argv[1] << " failed!" << std::endl;
        close(client_socket);
        return -1;
    }
    std::cout << "Connect to server " << argv[1] << std::endl;

    // Set range of outtime
    if (set_range_outtime(client_socket, 5) < 0){
        std::cerr << "Set socket opt failed!" << std::endl;
        close(client_socket);
        return -1;
    }

    // Init connect with smtp server
    if (connect_with_smtp(client_socket) < 0){
        close(client_socket);
        return -1;
    }

    // Use SSL connect
    SSL* ssl = connect_with_ssl(client_socket);
    if (ssl == nullptr){
        std::cerr << "Use SSL failed!" << std::endl;
        close(client_socket);
        return -1;
    }

    // send email to server
    std::cout << "Connect with mail application using SSL successfully!\nWait to send e-mail..." << std::endl;
    if (send_email_ssl(ssl) < 0){
        std::cerr << "Email send failed!" << std::endl;
        SSL_free(ssl);
        close(client_socket);
        return -1;
    }
    std::cout << "Email send successfully!" << std::endl;
    
    close(client_socket);
    return 0;
}