#include <iostream>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "comm.hpp"
#include <unistd.h>
#include <cassert>
using namespace std;
int main()
{
    //1. 不需创建管道文件，我只需要打开对应的文件即可！
    int wfd = open(fifoname.c_str(), O_WRONLY);
    if(wfd < 0)
    {
        std::cerr << errno << ":" << strerror(errno) << std::endl;
        return 1;
    }    
    // 可以进行常规通信了
    char buffer[NUM];
    while(true)
    {
        std::cout << "请输入你的消息# ";
        char *msg = fgets(buffer, sizeof(buffer) - 1, stdin); // 这里不用-1，fgets会自己处理，但是统一 -1，C的接口不用-1，但是其余的接口需要
        assert(msg);
        (void)msg;
        // int c = getch();
        // std::cout << c << std::endl;
        // if(c == -1) continue;

        // system("stty raw");
        // int c = getchar();
        // system("stty -raw");

        //std::cout << c << std::endl;
        //sleep(1);


        // 处理输入消息时获得的 \n 符号
        buffer[strlen(buffer) - 1] = 0;
        // abcde\n\0
        // 012345
        if(strcasecmp(buffer, "quit") == 0) break; // 忽略大小写进行比较

        ssize_t n = write(wfd, buffer, strlen(buffer)); // 这是往文件里面写不会关心是否有'\0'
        assert(n >= 0);
        (void)n;
    }
    close(wfd);

    return 0;
}