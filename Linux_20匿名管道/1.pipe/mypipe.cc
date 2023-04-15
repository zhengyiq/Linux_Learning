#include <iostream>
#include <string>
#include <cerrno>
#include <cassert>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
void Scene1(int pipefd[])
{
    // child
    // 3、关闭不需要的文件描述符，让父进程进行读取，子进程进行写入
    close(pipefd[0]);

    // 4、开始通信 -- 结合某种场景
    const std::string namestr = "hello, 我是子进程";
    int cnt = 1;
    char buffer[1024];
    while (true)
    {
        snprintf(buffer, sizeof(buffer), "%s, 计数器: %d, 我的PID: %d\n", namestr.c_str(), cnt++, getpid());
        write(pipefd[1], buffer, strlen(buffer));
        sleep(1);
    }

    close(pipefd[1]);
    exit(0);
}
void Scene2(int pipefd[])
{
    // child
    // 3、关闭不需要的文件描述符，让父进程进行读取，子进程进行写入
    close(pipefd[0]);

    // 4、开始通信 -- 结合某种场景
    const std::string namestr = "hello, 我是子进程";
    int cnt = 1;
    char buffer[1024];
    while (true)
    {
       char x = 'X';
       write(pipefd[1], &x, 1);
       std::cout << "Cnt:" << cnt++ << std::endl;
    }
    close(pipefd[1]);
    exit(0);
}

void Scene3(int pipefd[])
{
    // child
    // 3、关闭不需要的文件描述符，让父进程进行读取，子进程进行写入
    close(pipefd[0]);

    // 4、开始通信 -- 结合某种场景
    const std::string namestr = "hello, 我是子进程";
    int cnt = 1;
    char buffer[1024];
    while (true)
    {
        snprintf(buffer, sizeof(buffer), "%s, 计数器: %d, 我的PID: %d", namestr.c_str(), cnt++, getpid());
        write(pipefd[1], buffer, strlen(buffer));
        sleep(10);
    }

    close(pipefd[1]);
    exit(0);
}

void Scene4(int pipefd[])
{
    // child
    // 3、关闭不需要的文件描述符，让父进程进行读取，子进程进行写入
    close(pipefd[0]);

    // 4、开始通信 -- 结合某种场景
    int cnt = 0;
    while (true)
    {
        char x = 'X';
        write(pipefd[1], &x, 1);
        std::cout << "Cnt:" << cnt++ << std::endl;
        sleep(1);
        break;
    }

    close(pipefd[1]);
    exit(0);
}

void Scene5(int pipefd[])
{
    // child
    // 3、关闭不需要的文件描述符，让父进程进行读取，子进程进行写入
    close(pipefd[0]);

    // 4、开始通信 -- 结合某种场景
    int cnt = 0;
    while (true)
    {
        char x = 'X';
        write(pipefd[1], &x, 1);
        std::cout << "Cnt:" << cnt++ << std::endl;
        sleep(2);
    }

    close(pipefd[1]);
    exit(0);
}

int main()
{
    // 让不同的进程看到同一份资源
    // 任何一种进程间通信，一定要 先 保证不同的进程看到同一份资源
    int pipefd[2] = {0};
    // 1、创建管道
    int n = pipe(pipefd);    
    if (n < 0)
    {
        std::cout << "pipe error," << errno << ":" << strerror(errno) << std::endl;
        return 1;
    }
    std::cout << "pipefd[0]:" << pipefd[0] << std::endl; // 读端
    std::cout << "pipefd[1]:" << pipefd[1] << std::endl; // 写端
    
    // 2、创建子进程
    pid_t id = fork();
    assert(id != -1); // 正常应该使用判断，这里使用断言：意料之外使用if，意料之中使用assert        
    if (id == 0)
    {
        // Scene1(pipefd); // 正常通信场景
        // Scene2(pipefd); // 让子进程不断地去写入，但是父进程每隔10秒读一次
        // Scene3(pipefd); // 让子进程写慢一点，父进程读取不受限制
        // Scene4(pipefd); // 关闭写端
        // Scene5(pipefd); // 关闭读端
    }

    // parent
    // 3、关闭不需要的文件描述符，让父进程进行读取，子进程进行写入
    close(pipefd[1]);

    // 4、开始通信 -- 结合某种场景
    char buffer[1024];
    int cnt = 0;
    while (true)
    {
        // sleep(10); // Scene2 父进程每隔10秒进行读取 65535 2^16 
        // sleep(1); // Scene5
        int n = read(pipefd[0], buffer, sizeof(buffer) - 1); // 最多读1023个字符，需要预留出一个空位来赋给'\0'
        if (n > 0)
        {
            buffer[n] = '\0';
            std::cout << "我是父进程: child give me message: " << buffer << std::endl;
        }
        else if (n == 0)
        {
            std::cout << "我是父进程, 读到了文件结尾"<< std::endl;
            break;
        }
        else
        {
            std::cout << "我是父进程, 读异常了" << std::endl;
            break;
        }
        // if (cnt++ > 3) break; // Scene5 读端读一次就退出
    }
    close(pipefd[0]);
    int status = 0;
    waitpid(id, &status, 0);
    std::cout << "sig:" << (status &  0x7F) << std::endl; // 获取信号编号
    // std::cout << "sig:" << WTERMSIG(status) << std::endl;
    return 0;
}
// 管道的特点：
// 1、单向通信 -- 半双工
// 2、管道的本质是文件，因为fd的生命周期随进程，管道的生命周期也是随进程的
// 3、管道通信，通常用来进行具有血缘关系的进程，进行进程间通信。通常用与父子通信 -- pipe打开管道，并不清楚管道的名字，匿名管道
// 4、在管道通信中写入的次数，和读取的次数，不是严格匹配的 读写次数多少没有强相关 --- 表现 --- 字节流
// 5、具有一定的协同能力，让reader和writer能够按照一定的步骤进行通信 --- 自带同步机制

// 4种场景：
// 1、如果我们read读取完毕了所有的管道数据，如果对方不发，我们就只能等待 
// 2、如果我们writer将管道写满了，我们还能写吗？不能
// 3、如果我们关闭了写端，读取完毕管道数据，再读就会read返回0
// 4、写端一只写读端关闭，会发生什么？ 没有意义。OS不会维护无意义，低效率，或者浪费资源的事情。OS会杀死一直在写入的进程。OS会通过信号来终止进程 13)SIGPIPE

// 当要写入的数据量不大于PIPE_BUF时，linux将保证写入的原子性。
// 当要写入的数据量大于PIPE_BUF时，linux将不再保证写入的原子性。