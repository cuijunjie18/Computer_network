#include <iostream>
#include <unistd.h>
#include <string.h>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

int main()
{
    // std::string filePath = "saves/demo.txt";
    // std::ifstream input_file;
    // input_file.open(&filePath[0],std::ios::in);
    // char buf[1024];
    // auto filesize = input_file.tellg();
    // // input_file.seekg(0,std::ios::beg);
    // std::cout << filesize << "\n";
    // // input_file.read(buf, filesize);
    // // std::cout << buf << std::endl;
    fs::create_directories("123/456");
    return 0;
}