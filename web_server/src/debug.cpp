#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <filesystem>
#include <fstream>

#define MAXN 2048

// 构造404相应报文
void response_404(char** response_buf){
    std::string response =  "HTTP/1.1 404 Not Found\r\n"
                            "Content-Type: text/html\r\n"
                            "\r\n"
                            "<html><body><h1>404 Not Found</h1></body></html>";
    *response_buf = &response[0];
    printf("%s\n",*response_buf);
}

void Test1(char **p){
    char temp[20] = "987654321";
    *p = temp;
    std::cout << *p << "\n";
}

void Demo1(const std::string &filePath){
    std::ofstream file;
    file.open(filePath,std::ios::out | std::ios::app);
    char str[100] = "123456789";
    file << str;
    file.close();
}

void Demo2(const std::string &filePath){
    std::ifstream file;
    file.open(filePath,std::ios::in | std::ios::ate); // to end，才能统计大小
    // file >> data;
    // std::cout << data << "\n";
    // std::cout << "\n\n\n";
    auto filesize = file.tellg();
    std::cout << filesize  << "\n";

    char *data = (char *)std::malloc(MAXN * sizeof(char));
    file.seekg(0,std::ios::beg); // 移动到开头
    while (!file.eof()){ // 文件是按行读取的
        file >> data;
        std::cout << data;
    }
    file.close();
    return;
}

void Demo3(const std::string &filePath){
    std::ifstream fileObject(filePath,std::ios::in);

    int n = 1;

    // 定位到 fileObject 的第 n 个字节（假设是 ios::beg）
    fileObject.seekg( n );
     
    // 把文件的读指针从 fileObject 当前位置向后移 n 个字节
    fileObject.seekg( n, std::ios::cur );
      
    // 把文件的读指针从 fileObject 末尾往回移 n 个字节
    fileObject.seekg( n, std::ios::end );
       
    // 定位到 fileObject 的末尾
    fileObject.seekg( 0, std::ios::end );
}

int main(int argc,char *argv[])
{
    // std::string filePath = "bin/HelloWorld.html"; 
    return 0;
}