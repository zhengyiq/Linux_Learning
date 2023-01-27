#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
    while (1)
    {
        printf("本进程的PID是：%d，父进程的PPID是：%d\n", getpid(), getppid());
        sleep(1);
    }
    return 0;
}
