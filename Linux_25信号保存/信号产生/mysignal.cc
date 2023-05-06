#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;

void handler(int signo)
{
    cout << "我们的进程确实收到了：" << signo << " 信号导致崩溃的" << endl;
    cout << signo << endl;
    exit(1);
}

int main()
{
    pid_t id = fork();
    if (id == 0)
    {
        int* p = nullptr;
        // p = 100; // 这里将100当做地址写给p，但是编译器可能会报错
        *p = 100; // 会发生段错误，这里会向0号地址进行写入，但是我们并没有申请零号地址空间，因此会发生错误(野指针问题)

        cout << "野指针问题 ... " << endl;
        exit(0);
    }
    int status = 0;
    waitpid(id, &status, 0);
    cout << "exit code: " << ((status>>8) & 0xFF) << endl;
    cout << "exit signal: " << (status & 0x7F) << endl;
    cout << "core dump flag: " << ((status >> 7) & 0x1) << endl;

    // while (true)
    // {
    //     cout << "我是一个正常的进程，我正在模拟某种异常: " << getpid() << endl;
    //     sleep(1);
    // }
    // signal(SIGSEGV, handler);

    // int* p = nullptr;
    // // p = 100; // 这里将100当做地址写给p，但是编译器可能会报错
    // *p = 100; // 会发生段错误，这里会向0号地址进行写入，但是我们并没有申请零号地址空间，因此会发生错误(野指针问题)

    // cout << "野指针问题 ... " << endl;


    // signal(SIGFPE, handler);

    // int a = 0;
    // a /= 0;
    // cout << "div zero ... here" << endl;


    return 0;
}