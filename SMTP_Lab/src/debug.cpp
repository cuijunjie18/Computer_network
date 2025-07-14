#include "utils.h"

int main(int argc,char *argv[])
{
    char str[105] = "123142241";
    char *p = str;
    std::string s1(p); // 构造实现
    std::string s2 = p; // 赋值实现
    std::cout << s1 << "\n";
    std::cout << s2 << "\n";
    return 0;
}