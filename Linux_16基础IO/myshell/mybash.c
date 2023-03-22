#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX 1024
#define ARGC 64
#define SEP " "

// 一般用户自定义的环境变量，在bash中要用户自己来进行维护，不要用一个经常被覆盖的缓冲区来保存环境指令 


int split(char *commandstr, char *argv[])
{
    assert(commandstr);
    assert(argv);

    argv[0] = strtok(commandstr, SEP);
    if(argv[0] == NULL) return -1;
    int i = 1;
    while((argv[i++] = strtok(NULL, SEP)));
    //int i = 1;
    //while(1)
    //{
    //    argv[i] = strtok(NULL, SEP);
    //    if(argv[i] == NULL) break;
    //    i++;
    //}
    return 0;
}

void debugPrint(char *argv[])
{
    for(int i = 0; argv[i]; i++)
    {
        printf("%d: %s\n", i, argv[i]);
    }
}

void showEnv()
{
  extern char** environ;
  for (int i = 0; environ[i]; ++i)
  {
    printf("%d:%s\n", i, environ[i]);
  }
}

int main()
{
    int last_exit = 0;
    // 当我们在进行env查看的时候，我们想查看的是父进程bash的环境变量列表
    char myenv[32][256];
    int env_index = 0;
    while(1)
    {
        char commandstr[MAX] = {0};
        char *argv[ARGC] = {NULL};
        printf("[zyq@mymachine currpath]# ");
        fflush(stdout);
        char *s = fgets(commandstr, sizeof(commandstr), stdin);
        assert(s);
        (void)s; // 保证在release方式发布的时候，因为去掉assert了，所以s就没有被使用，而带来的编译告警, 什么都没做，但是充当一次使用
        // abcd\n\0
        commandstr[strlen(commandstr)-1] = '\0';
        // "ls -a -l" -> "ls" "-a" "-l" 字符串切割
        int n = split(commandstr, argv);
        if(n != 0) continue;
        //debugPrint(argv);
       

        // cd .. cd /  然bash自己执行的命令，我们称之为内建命令/内置命令
        if (strcmp(argv[0], "cd") == 0)
        {
          // 说到底，cd命令，重要的表现就如同bash自己调用了对应的函数
          if (argv[1] != NULL) chdir(argv[1]);
          continue;
        }

        // 原因：环境变量在export的时候是放在argv[1]中的，而argv字符串是一个变化的字符串，argv字符串指向的是commangstr中的内容，commandstr中的内容在不断地变化，未来在命令行获取的时候不断地被解析，字符串的起始地址会不断地被覆盖掉，因此环境变量一直在变化
        else if (strcmp(argv[0], "export") == 0) // 其实我们之前学习到的所有的(几乎)环境变量命令，都是内建命令
        {
          // if (argv[1] != NULL) putenv(argv[1]);
          if (argv[1] != NULL) 
          {
            strcpy(myenv[env_index], argv[1]);
            putenv(myenv[env_index++]);
          }
          continue;
        }
        else if (strcmp(argv[0], "env") == 0)
        {
          showEnv();
          continue;
        }
        else if (strcmp(argv[0], "echo") == 0)
        {
          // echo $PATH
          const char* target_env = NULL;
          if (argv[1][0] == '$') 
          {
            if (argv[1][1] == '?')
            {
              printf("%d\n",last_exit);
            }
            else 
            {
              target_env = getenv(argv[1] + 1);
            }
            if (target_env != NULL) printf("%s = %s\n", argv[1] + 1, target_env);
          }
          continue;
        }
        
        
        // verson 2
        if (strcmp(argv[0], "ls") == 0)
        {
          int pos = 0;
          while (argv[pos] != NULL) pos++;
          argv[pos++] = (char*)"--color=auto";
          argv[pos] = NULL;
        }
        
        
        // version 1
        pid_t id = fork();
        assert(id >= 0);
        (void)id;

        if(id == 0)
        {
            //child
            // execvpe(argv[0], argv, environ); 
            execvp(argv[0], argv);
            exit(1);
        }
        int status = 0;
        pid_t ret = waitpid(id, &status, 0);
        if (ret > 0)
        {
          last_exit = WEXITSTATUS(status);
        }
        
        //printf("%s\n", commandstr);
    }
}
