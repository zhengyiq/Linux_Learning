#include <stdio.h> 
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
// int add_to_top(int top)
// {
//   int sum = 0, i = 0;
//   for(i = 0; i <= top; ++i)
//   {
//     sum += i;
//   }
//   return sum;
// }

int main()
{
  pid_t id = fork();
  if (id == 0)
  {
    // child
    int cnt = 5;
    while(cnt)
    {
      printf("我是子进程，我还存活，还有%ds, pid:%d, ppid:%d\n", cnt--, getpid(), getppid());
      // int a = 0;
      // a /= 0;
      sleep(1);
    }
    exit(123);
  }
  // sleep(10);
  // pid_t ret_id = wait(NULL);
   while(1)
  {
    int status = 0;
    pid_t ret_id = waitpid(id, &status, WNOHANG);
    if(ret_id < 0)
    {
      printf("waitpid error!\n");
      exit(1);
    }
    else if(ret_id == 0)
    {
      printf("子进程还未退出，正在进行其他工作...\n");
      sleep(1);
      continue;
    }
    else 
    {     
       printf("我是父进程, 等待子进程信息, pid:%d, ppid:%d, ret_id:%d, child exit status:%d, child exit code:%d, child exit signal:%d\n", getpid(), getppid(), ret_id, status, (status>>8)&0xFF, status&0x7F);
       break;
    }
  }
  // sleep(5);
  
  // printf("hello world\n");
  // printf("hello world");
  // sleep(2);
  // _exit(123);


  // for (int i = 0; i <= 200; ++i)
  // printf("%d：%s\n", i, strerror(i));
  // exit(123);


  // int result = add_to_top(100); 
  // if (result == 5050) return 0;
  // else return 1;
}
