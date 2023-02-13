#include <stdio.h>
#include <unistd.h>
int main()
{
   // int count = 0;
   // while(1)
   // {
      // printf的本质就是外设打印消息，当CPU执行printf代码时，如果需要频繁打印，外设不一定是准备就绪的，因此进程就会在外设上进程排队，等到就绪在进行打印工作。
      // printf("正在运行! %d\n", count++);
      // sleep(1);
      // int a = 0;
      // scanf("%d", &a);
      // printf("%d\n", a);
      
   // }
   // int result = 1;
   // if (result == 10) return 0;
   // else return 3;
    
   // return 0;
   // 
   
   pid_t id = fork();
   if (id == 0)
   {
       // 子进程
       while (1)
       {
            printf("本进程是子进程PID：%d\n", getpid());
            sleep(2);
       }
   }
   else if (id > 0)
   {
       // 父进程
       while (1)
       {
            printf("本进程是父进程PID：%d\n", getpid());
            sleep(2);
       }
   }

}
