#include <stdio.h>
#include <unistd.h>
int main()
{
    pid_t id = fork();
    if (id == 0)
    {
        // child
        while(1)
        {  
            printf("这是子进程 pid：%d ppid：%d\n", getpid(), getppid());
            sleep(1);
        }
    }
    else
    { 
        // parent
        int cnt = 10;
        while(1)
        {  
            printf("这是父进程 pid：%d ppid：%d\n", getpid(), getppid());
            sleep(1);
            if (cnt-- <= 0) break;
        }
    }
    return 0;
}
