#ifndef  UTILS_HPP
#define UTILS_HPP

#include "common.hpp"

// Parse the domain name to sockaddr
in_addr parse_domain_name(char *domain_name);

// 字符串匹配 "\r\n\r\n"，复杂度o(4n)
int get_body(char *buf,int len);

// Set the range of outtime when receive response
int set_range_outtime(int &client_socket,int seconds);

// 判断文件是否存在
bool file_is_exist(std::string filePath);

#endif