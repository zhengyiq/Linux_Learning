#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
    int x = 100;
    pid_t ret = fork();
    if (ret == 0)
    {
        //子进程
        while (1)
        {
            printf("这是子进程 PID是：%d，父进程的PPID是：%d %d %p\n", getpid(), getppid(), x, &x);
            x = 200;
            sleep(1);
        }
    }
    else if (ret > 0)
    {
        //父进程
        while (1)
        {
            printf("这是父进程 PID是：%d，父进程的PPID是：%d %d %p\n", getpid(), getppid(), x, &x);
            x = 500;
            sleep(1);
        }
    }
    else
    {}
//    while (1)
//    {
//        printf("本进程的PID是：%d，父进程的PPID是：%d\n", getpid(), getppid());
//        sleep(1);
//    }

    return 0;
}
