#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cassert>
#include <cstring>

using namespace std;
static void PrintPending(const sigset_t &pending)
{
    cout << "当期进程的pending信号集:";
    for (int signo = 1; signo <= 31; ++signo)
    {
        if (sigismember(&pending, signo)) // 用于打印信号集
            cout << "1";
        else
            cout << "0";
    }
    cout << endl;
}

static void handler(int signo) // 添加了static之后该函数只能在本文件中使用
{
    cout << "对特定信号：" << signo << "执行捕捉动作" << endl;
    int cnt = 10;
    while (cnt)
    {
        cnt--;
        sigset_t pending;
        sigemptyset(&pending);
        sigpending(&pending);
        PrintPending(pending);
        cout << "打印完成pending信号集" << endl;
        sleep(1);
    }
}
int main()
{
    struct sigaction act, oldact;
    memset(&act, 0, sizeof(act));
    memset(&oldact, 0, sizeof(oldact));
    act.sa_handler = handler;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, 3); // 可以添加其他信号的阻塞方式，在自定义捕捉时将其余收到的信号阻塞
    sigaddset(&act.sa_mask, 4);
    sigaddset(&act.sa_mask, 5);
    sigaction(2, &act, &oldact);

    while (true)
    {
        cout << getpid() << endl;
        sleep(1);
    }
}
