#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <sys/select.h>
#include "Log.hpp"
#include "Sock.hpp"

const static int gport = 8888;
typedef int type_t;
// static const int defaultfd = -1; 

class SelectServer
{
    static const int N = (sizeof(fd_set) * 8);
public:
    SelectServer(uint16_t port = gport) : port_(port) {}

    void InitServer()
    {
        listensock_.Socket();
        listensock_.Bind(port_);
        listensock_.Listen();

        for (int i = 0; i < N; ++i)
        {
            fdarray_[i] = defaultfd;
        }
    }

    void Accepter()
    {
        logMessage(Debug, "%s", "有一个新链接到来了", errno, strerror(errno));
        // Accept，这里不会再被阻塞
        std::string clientip;
        uint16_t clientport;
        int sock = listensock_.Accept(&clientip, &clientport);
        if (sock < 0)
            return;
        // 得到了对应的sock，这里我们可不可以进行read/recv，读取sock?
        // sock上不一定有数据就绪，所以我们需要将sock交给select，让select进行管理
        logMessage(Debug, "[%s:%d], sock: %d", clientip.c_str(), clientport, sock);
        // 要让select 帮我们关心，只要把sock添加到fdarray_[]即可
        int pos = 1;
        for (; pos < N; ++pos)
        {
            if (fdarray_[pos] == defaultfd)
                break;
        }
        if (pos >= N)
        {
            close(sock);
            logMessage(Warning, "sockfd array[] full");
        }
        else
        {
            fdarray_[pos] = sock;
        }
    }

    void HandlerEvent(fd_set rfds)
    {
        for (int i = 0; i < N; ++i)
        {
            if (fdarray_[i] == defaultfd) continue;

            if ((fdarray_[i] == listensock_.Fd()) && FD_ISSET(listensock_.Fd(), &rfds))
            {
                Accepter();
            }
            else if ((fdarray_[i] != listensock_.Fd()) && FD_ISSET(fdarray_[i], &rfds))
            {
                // ServiceIO();
                // BUG
                int fd = fdarray_[i];
                char buffer[1024]; 
                ssize_t s = recv(fdarray_[i], buffer, sizeof(buffer) - 1, 0); // 读取不会被阻塞 这里假定一次就将数据读完
                if (s > 0)
                {
                    buffer[s-1] = 0;
                    buffer[s-2] = 0;
                    std::cout << "client# " << buffer << std::endl;

                    // 发送回去也要被select管理
                    std::string echo = buffer;
                    echo += "[select server echo]";       
                    send(fd, echo.c_str(), echo.size(), 0);             
                }
                else 
                {
                    if (s == 0)
                        logMessage(Info, "client quit ... fdarray_[i] -> defaultfd: defaultfd: %d->%d", fd, defaultfd);
                    else
                        logMessage(Warning, "recv error, client quit ... fdarray_[i] -> defaultfd: defaultfd: %d->%d", fd, defaultfd);
                    close(fdarray_[i]);
                    fdarray_[i] = defaultfd;
                }
            }
        }
    }

    void Start()
    {
        // 1. 这里能够直接获取链接吗?
        // 2. 最开始的时候，我们的服务器是没有太多的sock的，甚至只有一个sock listensock_
        // 3. 在网络当中，新链接到来被当做读事件就绪！ 
        // 不能 listensock_.Accept();
        
        // // demo 0
        // fd_set rfds;
        // FD_ZERO(&rfds);
        // FD_SET(listensock_.Fd(), &rfds);
        // while (true)
        // {
        //     // struct timeval timeout = {2, 0}; // 每次需要对这个数值进行重置
        //     // 因为rfds是一个输入输出型参数，注定了每次都要对rfds进行重置，必定要知道历史上都有哪些fd在fdarray_[]中
        //     // 因为服务器在运行中，sockfd的值一直在动态变化，所以maxfd也一定在变化，maxfd也需要进行动态更新 fdarray_[]
        //     int n = select(listensock_.Fd() + 1, &rfds, nullptr, nullptr, nullptr);
        //     switch (n)
        //     {
        //     case 0:
        //         logMessage(Debug, "timeout, %d: %s", errno, strerror(errno));
        //         break;
        //     case -1:
        //         logMessage(Warning, "%d: %s", errno, strerror(errno));
        //         break;
        //     default:
        //         // 成功了
        //         logMessage(Debug, "有一个就绪事件发生了: %d", n);
        //         HandlerEvent(rfds);
        //         break;
        //     }
        //     sleep(1);
        // }

        // demo 1
        fdarray_[0] = listensock_.Fd();
        while (true)
        {
            fd_set rfds;
            FD_ZERO(&rfds);
            int maxfd = fdarray_[0];
            for (int i = 0; i < N; ++i)
            {
                if (fdarray_[i] == defaultfd) continue;
                FD_SET(fdarray_[i], &rfds);
                if (maxfd < fdarray_[i]) maxfd = fdarray_[i];
            }
     
            int n = select(maxfd + 1, &rfds, nullptr, nullptr, nullptr);
            switch (n)
            {
            case 0:
                logMessage(Debug, "timeout, %d: %s", errno, strerror(errno));
                break;
            case -1:
                logMessage(Warning, "%d: %s", errno, strerror(errno));
                break;
            default:
                // 成功了
                logMessage(Debug, "有一个就绪事件发生了: %d", n);
                HandlerEvent(rfds);
                DebugPrint();
                break;
            }
        }
    }

    void DebugPrint()
    {
        std::cout << "fdarray_[]: ";
        for (int i = 0; i < N; i++)
        {
            if (fdarray_[i] == defaultfd) continue;
            std::cout << fdarray_[i] << " ";
        }
        std::cout << "\n";
    }

    ~SelectServer()
    {
        listensock_.Close();
    }
    
private:
    uint16_t port_;
    Sock listensock_;
    type_t fdarray_[N]; // 管理所有的fd
};

