#include <stdio.h> 
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#define TASK_NUM 10

// 预设的任务
void sync_disk()
{
  printf("这是一个刷新数据的任务!\n");
}

void sync_log()
{
  printf("这是一个同步日志的任务!\n");
}

void net_send()
{
  printf("这是一个进行网络发送的任务!\n");
}

// 要保存的任务相关
typedef void (*func_t)(); // 函数指针类型
func_t other_task[TASK_NUM] = {NULL}; // 函数指针数组

void InitTask()
{
  for(int i = 0; i < TASK_NUM; ++i) other_task[i] = NULL;
}

int LoadTask(func_t func)
{
  int i = 0;
  for(i = 0; i < TASK_NUM; ++i)
  {
    if(other_task[i] == NULL) break;
  }
  if (i == TASK_NUM) return -1;
  else other_task[i] = func;

  return 0;
}

void RunTask()
{
  for(int i = 0; i < TASK_NUM; ++i)
  {
    if (other_task[i] != NULL)
    {
      other_task[i]();
    }
  }
}

int main()
{
  pid_t id = fork();
  if (id == 0)
  {
    // child
    int cnt = 100;
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
  // parent
  // pid_t ret_id = wait(NULL);
  InitTask();
  LoadTask(sync_disk);
  LoadTask(sync_log);
  LoadTask(net_send);
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
      // printf("子进程还未退出，正在进行其他工作...\n");
      RunTask();
      sleep(1);
      continue;
    }
    else 
    {     
      //  printf("我是父进程, 等待子进程信息, pid:%d, ppid:%d, ret_id:%d, child exit status:%d, child exit code:%d, child exit signal:%d\n", getpid(), getppid(), ret_id, status, (status>>8)&0xFF, status&0x7F);
      if(WIFEXITED(status))
      {
        printf("wait success, child exit code:%d\n", WEXITSTATUS(status));
      }
      else 
      {
        printf("wait success, child exit singal:%d\n", WIFSIGNALED(status));
      }
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
