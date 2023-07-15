#pragma once
#include "err.hpp"
#include <iostream>
#include <string>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <functional>
namespace ns_server
{
    const static uint16_t default_port = 8080;
    
    using func_t = std::function<std::string (std::string)>;
    class UdpServer
    {
    public:
        UdpServer(func_t cb, uint16_t port = default_port) : service_(cb), port_(port)
        {
            std::cout << "server addr: " << port_ << std::endl;
        }

        void InitServer()
        {
            // 1. 创建socket接口，打开网络文件
            sock_ = socket(AF_INET, SOCK_DGRAM, 0); // AF_INET 网络通信 SOCK_DGRAM UDP服务器
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
            // 3. 云服务器，或者一款服务器，一般不要指明某一个确定的IP -- 服务器可能有多张网卡，可能配有多个IP
            local.sin_addr.s_addr = INADDR_ANY; // 让我们的udpserver在启动的时候，bind本主机上的任意IP
            // 这里需要将字符串转换uint32_t的类型，并且同时进行将主机序列转换成网络序列

            if (bind(sock_, (struct sockaddr*)&local, sizeof(local)) < 0)
            {
                std::cerr << "bind socket error: " << strerror(errno) << std::endl;
                exit(BIND_ERR);
            }

            std::cout << "bind socket success: " << sock_ << std::endl; //3
        }

        void Start()
        {
            // 服务器的正常工作
            char buffer[1024];
            while (true)
            {
                // 收
                // ssize_t 实际写入的大小 recvfrom(int sockfd 绑定的套接字, void *buf 接受数据存放的缓冲区, size_t len 缓冲区长度, int flags 读取方式(0), struct sockaddr *src_addr 需要知道client的IP和PORT 输入接收缓冲区, socklen_t *addrlen 实际结构体的大小); 输入输出型参数 需要知道谁发的数据
                struct sockaddr_in peer;
                socklen_t len = sizeof(peer); // 这里一定要写清楚，未来传入的缓冲区的大小
                int n = recvfrom(sock_, buffer, sizeof(buffer)-1/*因为这是以字符作为消息的类型，所以缓冲区预留一部分空间*/, 0, (struct sockaddr*)&peer, &len); // 消息的类型需要程序员来定义
                if (n > 0) buffer[n] = '\0';
                else continue;

                // 提取client信息
                std::string clientip = inet_ntoa(peer.sin_addr); // 把一个四字节的IP转化为字符串
                uint16_t clientport = ntohs(peer.sin_port); // 将从网络中获取的端口号转换成主机
                std::cout << clientip << "-" << clientport << "#" << buffer << std::endl;

                // 做业务处理
                std::string response = service_(buffer);

                // 发
                sendto(sock_, response.c_str(), response.size(), 0, (struct sockaddr*)&peer, len); // 往文件中去写的时候，不需要携带\0
            }

        }

        ~UdpServer()
        {}
    private:
        int sock_;
        uint16_t port_;
        func_t service_; // 我们的网络服务器刚刚解决的是网络IO的问题，要进行业务处理
        // std::string ip_; // 后面需要去掉这个ip
    };
}