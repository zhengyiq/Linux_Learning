#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

void handler(int signo)
{
    cout << "我们的进程确实是收到了: " << signo << " 信号导致崩溃的" << endl;
    exit(1);
}

void showBlock(sigset_t *oset)
{
    int signo = 1;
    for(; signo <=31; signo++)
    {
        if(sigismember(oset, signo)) cout << "1";
        else cout << "0";
    }
    cout << endl;
}

int main()
{
    // 只是在用户层面上进行设置
    sigset_t set, oset; // int a;
    sigemptyset(&set);
    sigemptyset(&oset);
    sigaddset(&set, 2); //SIGINT

    // 设置进入进程，谁调用，设置谁
    sigprocmask(SIG_SETMASK, &set, &oset); // 1. 2号信号没有反应 2. 我们看到老的信号屏蔽字block位图是全零、
    int cnt = 0;
    while(true)
    {
        showBlock(&oset);
        sleep(1);
        cnt++;

        if(cnt == 3)
        {
            cout << "recover block" << endl;
            sigprocmask(SIG_SETMASK, &oset, &set);
            showBlock(&set); //下节课
        }
    }

    return 0;
}

// int main()
// {
//     // #define SIG_DFL ((__sighandler_t)0) /* Default action.  */
//     // #define SIG_IGN ((__sighandler_t)1) /* Ignore signal.  */
//     signal(2, SIG_IGN);
//     while (true)
//     {
//         sleep(1);
//     }
//     return 0;
// }