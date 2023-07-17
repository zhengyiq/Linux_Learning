#pragma once

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <functional>

#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include "err.hpp"

namespace ns_server
{
    static const uint16_t defaultport = 8081;
    static const int backlog = 32;

    using func_t = std::function<std::string(std::string)>;

    class TcpServer;
    class ThreadData
    {
    public:
        ThreadData(int fd, const std::string &ip, const uint16_t &port, TcpServer *ts)
            : sock(fd), clientip(ip), clientport(port), current(ts)
        {}

    public:
        int sock;
        std::string clientip;
        uint16_t clientport;
        TcpServer *current;
    };

    class TcpServer
    {
    public:
        TcpServer(func_t func, uint16_t port = defaultport) : func_(func), port_(port), quit_(true)
        {
        }

        void initServer()
        {
            // 1. 创建socket，文件
            listensock_ = socket(AF_INET, SOCK_STREAM, 0); // 创建套接字
            if (listensock_ < 0)
            {
                std::cerr << "create socket error" << std::endl;
                exit(SOCKET_ERR);
            }

            // 2. bind
            struct sockaddr_in local;
            memset(&local, 0, sizeof(local));
            local.sin_family = AF_INET;
            local.sin_port = htons(port_); // 收发消息的时候，没有做主机转网络，会自动做大小端转化
            local.sin_addr.s_addr = htonl(INADDR_ANY);
            if (bind(listensock_, (struct sockaddr *)&local, sizeof(local)) < 0)
            {
                std::cerr << "bind socket error" << std::endl;
                exit(BIND_ERR);
            }

            // 3. 监听 客户端要链接服务器
            if (listen(listensock_, backlog) < 0)
            {
                std::cerr << "listen socket error" << std::endl;
                exit(LISTEN_ERR);
            }
        }

        static void* threadRoutine(void* args)
        {
            pthread_detach(pthread_self()); // 为了不用join将线程自己进行分离
            ThreadData* td = static_cast<ThreadData*>(args);
            td->current->service(td->sock, td->clientip, td->clientport);
            delete td;
            return nullptr;
        }

        void start()
        {
            quit_ = false;
            while (!quit_)
            {
                struct sockaddr_in client;
                socklen_t len = sizeof(client);

                // 4. 获取连接 accept
                int sock = accept(listensock_, (struct sockaddr *)&client, &len); // 返回值是一个文件描述符，一个描述符专门用于监听，另一个描述符专门用于套接字通信
                if (sock < 0)
                {
                    std::cerr << "accept error" << std::endl;
                    continue; // 获取连接失败并不需要终止程序
                }

                // 提取client信息
                std::string clientip = inet_ntoa(client.sin_addr);
                uint16_t clientport = ntohs(client.sin_port);

                // 5. 获取新连接成功，开始业务处理
                std::cout << "获取新连接成功: " << sock << " from " << listensock_ << ", " << clientip << "-" << clientport << std::endl;

                // v3: 多线程 -- 原生多线程
                // 1. 要不要关闭不要的socket？？不能
                // 2. 要不要回收线程？如何回收？会不会阻塞？？
                pthread_t tid;
                ThreadData* td = new ThreadData(sock, clientip, clientport, this);
                pthread_create(&tid, nullptr, threadRoutine, td); // 要给线程传入sock...

                // pthread_join();
            }
        }

        // 流式操作可以使用read进行读取数据,UDP不是流式的是面向数据报的
        void service(int sock, const std::string &clientip, const uint16_t &clientport)
        {
            std::string who = clientip + "-" + std::to_string(clientport);
            char buffer[1024];
            while (true)
            {
                ssize_t s = read(sock, buffer, sizeof(buffer) - 1);
                if (s > 0)
                {
                    buffer[s] = 0;
                    std::string res = func_(buffer); // 进行回调
                    std::cout << who << " >>> " << res << std::endl;

                    write(sock, res.c_str(), res.size());
                }
                else if (s == 0)
                {
                    // 对方将连接关闭了
                    close(sock);
                    std::cout << who << " quit, me too" << std::endl;
                    break;
                }
                else
                {
                    close(sock);
                    std::cerr << "read error: " << strerror(errno) << std::endl;
                    break;
                }
            }
        }

        ~TcpServer()
        {
        }

    private:
        uint16_t port_;
        int listensock_;
        bool quit_;
        func_t func_;
    };
}