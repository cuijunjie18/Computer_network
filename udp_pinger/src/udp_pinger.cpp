#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>

#include <random>
#include <time.h>

#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <string.h>

#include <chrono>
#include <inttypes.h>

#include <algorithm>

#define MAXN 2048

int main(int argc,char *agrv[])
{
    // Generate socket
    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket < 0){
        std::cerr << "Generate socket failed!" << std::endl;
        return -1;
    }

    // Set the server addr
    std::string server_ip = "127.0.0.1";
    uint16_t server_port = 8001;
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, &server_ip[0], &server_addr.sin_addr) < 0){ // 注意最后的地址要到sin_addr
        std::cerr << "Invalid server ip!" << std::endl;
        return -1;
    }

    // Set range of outtime
    struct timeval tv;
    tv.tv_sec = 1; // 1s
    tv.tv_usec = 0;
    if (setsockopt(client_socket,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv)) < 0){
        std::cerr << "Set socket opt failed!" << std::endl;
        close(client_socket);
        return -1;
    }

    // define send variable
    std::string send_data;
    char recv_buf[MAXN];
    bool recv = false;
    int packet_nums = 10;
    int recv_nums = 0;
    double min_duration = 999999999999;
    double max_duration = 0;
    double avg_duration = 0;
    
    // Execute ping 10 times
    std::cout << "PING" << " " << server_ip << std::endl;
    auto bbegin_time = std::chrono::high_resolution_clock::now(); 
    for (int i = 0; i < packet_nums; i++){
        // 获取时间戳，构造发送数据
        std::chrono::time_point now = std::chrono::system_clock::now();
        std::time_t timestamp = std::chrono::system_clock::to_time_t(now);
        send_data = "Ping " + std::to_string(i + 1) + " " + std::to_string(timestamp);

        // Send
        // std::cout << "PING " << server_ip << "     " << send_data.size() << " bytes of data." << std::endl;
        auto send_time = std::chrono::high_resolution_clock::now(); // 高精度统计时间
        if (sendto(client_socket, &send_data[0], send_data.size(), 0, 
            (struct sockaddr *) &server_addr,sizeof(server_addr)) < 0){
                std::cerr << "Send failed!" << std::endl;
                break;
        }

        // recv
        std::cout << "icmp_seq=" << i + 1 << " ";
        int bytes_recv = recvfrom(client_socket, recv_buf, MAXN, 0, nullptr, nullptr);

        auto recv_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(recv_time - send_time); // us为单位
        if (bytes_recv > 0){
            printf("Recv %d bytes from server. RTT=%.3fms\n",bytes_recv,duration.count() / 1000.0); // convert to ms
            recv_nums++;
            min_duration = std::min((double)(duration.count()),min_duration);
            max_duration = std::max((double)(duration.count()),max_duration);
            avg_duration += duration.count();
        }else{
            std::cout << "Request timed out!" << std::endl;
        }
    }

    // Output statistics
    auto eend_time = std::chrono::high_resolution_clock::now();
    auto all_duration = std::chrono::duration_cast<std::chrono::microseconds>(eend_time - bbegin_time);
    double loss_rate = static_cast<double>(packet_nums - recv_nums) / packet_nums; 
    printf("--- %s ping statistics ---\n",&server_ip[0]);
    printf("%d packets transmitted, %d received, %.3f%% packet loss, time %.3f ms\n",
        packet_nums,recv_nums,loss_rate * 100,all_duration.count() / 1000.0);
    printf("rtt min/avg/max = %.3f/%.3f/%.3f ms\n",
        min_duration / 1000,max_duration / 1000, avg_duration / 1000 / recv_nums);
    close(client_socket);
    return 0;
}