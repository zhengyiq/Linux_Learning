// int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
// 输入型 int nfds: select等待的多个fd数字层面最大的fd + 1
// 输入输出型 fd_set *readfds: 
    // fd_set 用位图结构表示用户 <-> 内核之间信息的互相传递
    // 1. 用户首先告诉内核，哪些fd需要关心读事件
    // 2. 内核告诉用户，哪些读事件已经就绪
    // 用户->内核 fd_set rfds 1111 1101
    // 内核->用户 fd_set rfds 1000 0001
// 输入输出型 struct timeval *timeout: 1. NULL 阻塞等待 2. {0，0} 非阻塞等待 3. {n, m} {5, 0} 5秒内阻塞等待，否则timeout一次(进行一次非阻塞)
    // struct timeval
    // {
    //     time_t tv_sec;       /* seconds */
    //     suseconds_t tv_usec; /* microseconds */
    // };
// int n 1. n > 0: 有几个fd就绪 2. n == 0: timeout 3. n < 0: 等待失败
#include "SelectServer.hpp"
#include <memory>
int main()
{
    // fd_set fd;
    // std::cout << sizeof(fd) * 8 << std::endl;

    std::unique_ptr<SelectServer> svr(new SelectServer());

    svr->InitServer();
    svr->Start();
    return 0;
}
