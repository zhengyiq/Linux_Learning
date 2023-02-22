#include <stdio.h>
#include <unistd.h>
#include <assert.h>
int main()
{
    pid_t id = fork();
    assert(id >= 0);
    int cnt = 100;
    if (id == 0)
    {
        // child
        while (1)
        {  
            printf("this is child pid:%d ppid:%d cnt = %d &cnt = %p\n", getpid(), getppid(), cnt, &cnt);
            sleep(1);
            cnt = 200;

        }
    }
    else 
    {
        // parent
        while (1)
        {
            printf("this is parent pid:%d ppid:%d cnt = %d &cnt = %p\n", getpid(), getppid(), cnt, &cnt);
            sleep(1);
        }
    }

    return 0;
}
