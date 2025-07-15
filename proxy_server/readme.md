# HTTP Web Proxy Server Lab

## 说明

仅支持单次TCP连接的代理服务器，不支持并发，且仅支持第一层目录请求.

- [ ] 支持并发.
- [ ] 增强服务器稳定性的错误检查，特别是文件检查.
- [ ] 对应404等非获取文件请求，支持统一归类存到cache，而不是将每次的非法响应存起来.
- [ ] 支持跨平台，避免使用特定操作系统的系统函数.
- [ ] 支持多层文件请求

## 收获

- Linux系统函数——文件操作
  Linux系统中几乎所有设备、普通文件、socket都是文件.

  读文件
  ```cpp
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <unistd.h>

  char buf[MAXN];
  int fd = open(file_path, O_RDONLY, 0);
  int filesize = read(fd, buf, MAXN); // 这里的字节数在少于MAXN下为文件的字节数大小
  ```

  写文件
  ```cpp
  char file_path[105] = "saves/demo.html";
  int fd = open(file_path, O_WRONLY | O_CREAT, 0644); // 设置文件权限，0644，0不可省略，仅文件不存在时有效.
  char demo[105] = 
      "<html>\n"
      "   Hello,World!\n"
      "</html>";
  if (write(fd, demo, strlen(demo)) < 0){
      std::cerr << "Fail!" << std::endl;
      return -1;
  }
  ```

  **注意：上面这种文件操作属于系统函数调用的范畴，通常不具备平台的迁移性.**
  
- c++ 文件流处理文件
  读文件
  ```cpp
  
  ```
  参考文章：https://www.runoob.com/cplusplus/cpp-files-streams.html

- c++17 filesystem 递归创建目录
  ```cpp
  #include <fstream>
  namespace fs = std::filesystem;
  fs::create_directories("123/456");
  ```

- c++ 类的析构函数的调用问题
  exit(1)会调用全局对象的析构
  ```cpp
  ProxyServer global_server(8080); // 全局对象

  int main() {
      // ...
      exit(1); // 会调用 global_server 的析构函数
  }
  ```
  exit(1)不会调用局部对象的析构
  ```cpp
  int main() {
    ProxyServer local_server(8080); // 局部对象
    exit(1); // 不会调用 local_server 的析构函数！
  }
  ```
  故不要在构造里使用exit(1)，**不安全!**