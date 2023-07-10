#pragma once

#include <iostream>
#include <string>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
namespace ns_server
{
    enum{
        SOCKET_ERR = 1,
        BIND_ERR
    };
    
    const static uint16_t default_port = 8080;
    
    class UdpServer
    {
    public:
        UdpServer(std::string ip, uint16_t port = default_port) : ip_(ip), port_(port)
        {
            std::cout << "server addr: " << ip << " : " << port_ << std::endl;
        }

        void InitServer()
        {
            // 1. 创建socket接口，打开网络文件
            sock_ = socket(AF_INET, SOCK_DGRAM, 0);
            if (sock_ < 0)
            {
                std::cerr << "create socket error: " << strerror(errno) << std::endl;
                exit(SOCKET_ERR);
            }
            std::cout << "create socket success: " << sock_ << std::endl; // 3

            // 2. 给服务器指明IP地址和端口号Port
            struct sockaddr_in local; // 这个 local 在哪里定义呢？用户空间的特定函数的栈帧上，不在内核中！ 
            bzero(&local, sizeof(local)); // 清空上述字段ud
            
            local.sin_family = AF_INET; // PF_INET 初始化socketaddr_in结构
            local.sin_port = htons(port_); // 本地主机序列构建的port_，需要从主机序列转变成网络序列
            // inet_addr: 1,2
            // 1. 字符串风格的IP地址，转换成为4字节int, "1.1.1.1" -> uint32_t -> 能不能强制类型转换呢？不能，这里要转化
            // 2. 需要将主机序列转化成为网络序列
            local.sin_addr.s_addr = inet_addr(ip_.c_str()); // sin_addr C++中的结构体在C++中可以进行转化，但是在C语言中不行
            // 这里需要将字符串转换uint32_t的类型，并且同时进行将主机序列转换成网络序列

            if (bind(sock_, (struct sockaddr*)&local, sizeof(local)) < 0)
            {
                std::cerr << "bind socket error: " << strerror(errno) << std::endl;
                exit(BIND_ERR);
            }

            std::cout << "bind socket success: " << sock_ << std::endl; //3

        }

        void Start()
        {}

        ~UdpServer()
        {}
    private:
        int sock_;
        uint16_t port_;
        std::string ip_; // 后面需要去掉这个ip
    };
}