#include <chrono>
#include <iostream>

int main()
{
    std::chrono::time_point time = std::chrono::system_clock::now();
    std::time_t now = std::chrono::system_clock::to_time_t(time);
    std::cout << now << "\n";
    return 0;
}