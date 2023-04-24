#include <iostream>
#include <unistd.h>
#include <cassert>
#include <signal.h>
#include <sys/types.h>
#include <string>
#include <cstring>
#include <cstdlib>
int count = 0;

void Usage(std::string)
{
    std::cout << "Usage: \n\t";
    std::cout << "信号编号 目标进程\n" << std::endl;
}

void myhandler(int signo)
{
    std::cout << "get a signal: " << signo << " pid: " << getpid() << std::endl;
    int n = alarm(10);
    // std::cout << count << std::endl;
    std::cout << "return: " << n << std::endl;
    // exit(1);
}

// ./mykill -9 -1234
int main(int argc, char** argv)
{
    // IO的效率其实非常底下
    signal(SIGALRM, myhandler);
    alarm(10);
    while (true)
    {
        sleep(1);
        // count++;
        // 打印，显示器打印，网络，IO
        // std::cout << count++ << std::endl; // 1s我们的计算机会将一个整数累计到多少
    }


    // while (true)
    // {
    //     signal(SIGABRT, myhandler);
    //     std::cout << "begin" << std::endl;
    //     abort();
    //     std::cout << "end" << std::endl;
    // }

    // while (true)
    // {
    //     signal(SIGINT, myhandler);
    //     sleep(1);
    //     raise(2);
    // }

    // if (argc != 3)
    // {
    //     Usage(argv[0]);
    //     exit(1);
    // }
    // int signo = atoi(argv[1]);
    // int target_id = atoi(argv[2]);

    // int n = kill(target_id, signo);
    // if (n != 0)
    // {
    //     std::cerr << errno << " : " << strerror(errno) << std::endl;
    //     exit(2);
    // }
}


//终端按键产生信号=============================================================================================================================================================
// // 我们平时在输入的时候，计算机怎样知道我从键盘输入了数据？键盘是通过硬件中断的方式，通知系统，我们的键盘已经按下

// // 自定义方法
// // signo: 当特定的信号被发送给当前进程的时候，执行handler方法的时候，要自动填充对应的信号给handler方法

// void handler(int signo)
// {
//     std::cout << "get a singal:" << signo << std::endl;
// }

// int main()
// {
//     // 1. 2号信号，进程默认的处理动作是终止进程；
//     // 2. signal 可以对指定的信号设置自定义处理动作；
//     // 3. signal(2，handler)调用完这个函数的时候，handler方法调用了吗？没有，做了什么，只是更改了2号信号的处理动作，并没有调用该方法；
//     // 4. 那么handler方法，什么时候被调用？当2号信号产生的时候。
//     // 5. 默认对2号信号的处理动作终止进程，我们用signal(2，handler)，我们在执行用户动作自定义捕捉
    
//     // signal(SIGINT, handler);
//     // signal(2, handler);
//     for (int i = 1; i < 32; ++i)
//     {
//         signal(i, handler); 
//     }

//     while (true)
//     {
//         std::cout << "我是一个进程，我正在运行 ..., pid:" << getpid() << std::endl;
//         sleep(5);
//     }
// }
//=============================================================================================================================================================
