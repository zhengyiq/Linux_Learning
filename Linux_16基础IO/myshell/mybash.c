#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX 1024
#define ARGC 64
#define SEP " "

enum redir {
  REDIR_INPUT = 0,
  REDIR_OUTPUT,
  REDIR_APPEND,
  REDIR_NONE
}redir_type;

char* checkDir(char* commandstr, enum redir* redir_type)
{
  // 检测commandstr内部是否有 < > >> 
  // 如果有要根据 > >>< 设置称为两部分
  // 将 > < >> -> \0，将commandstr设置成为两部分
  // 保存文件名并返回
  // 如果上面条件不满足直接返回
  int flag = 0;
  int end = strlen(commandstr) - 1;
  int i = 0;
  for (i = end; i > -1; --i)
  { 
    if (commandstr[i] == '>' && commandstr[i - 1] == '>')
    {
      flag = 1;
      *redir_type = REDIR_APPEND; 
      commandstr[i-1] = '\0';
      break;
    }
    else if(commandstr[i] == '<')
    {
      flag = 1;
      *redir_type = REDIR_OUTPUT;
      commandstr[i] = '\0';
      break;
    }
    else if (commandstr[i] == '>') 
    {
      flag = 1;
      *redir_type = REDIR_INPUT;
      commandstr[i] = '\0';
      break;
    }
  }

  if (flag == 1)
  {
    return commandstr + i + 1;
  }
  else 
  {
    return NULL;
  }
}

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
        redir_type = REDIR_NONE;
        char commandstr[MAX] = {0};
        char *argv[ARGC] = {NULL};
        printf("[zyq@mymachine currpath]# ");
        fflush(stdout);
        // cat Makefile > log.txt 
        // cat Makefile >> log.txt 
        // cat Makefile < log.txt 
        // 从右往左找 先判断是否有
        
        char *s = fgets(commandstr, sizeof(commandstr), stdin);
        assert(s);
        (void)s; // 保证在release方式发布的时候，因为去掉assert了，所以s就没有被使用，而带来的编译告警, 什么都没做，但是充当一次使用
        // abcd\n\0
        commandstr[strlen(commandstr)-1] = '\0';
        char* filename = checkDir(commandstr, &redir_type);
        // "ls -a -l" -> "ls" "-a" "-l"
        int n = split(commandstr, argv);
        if(n != 0) continue;
        //debugPrint(argv);
        // cd .. / cd /: 让bash自己执行的命令，我们称之为 内建命令/内置命令
        if(strcmp(argv[0], "cd") == 0)
        {
            //说到底，cd命令，重要的表现就如同bash自己调用了对应的函数
            if(argv[1] != NULL) chdir(argv[1]);
            continue;
        }
        else if(strcmp(argv[0], "export") == 0) // 其实我们之前学习到的所有的(几乎)环境变量命令，都是内建命令
        {
            if(argv[1] != NULL){
                strcpy(myenv[env_index], argv[1]);
                putenv(myenv[env_index++]);
            }
            continue;
        }
        else if(strcmp(argv[0], "env") == 0)
        {
            showEnv();
            continue;
        }
        else if(strcmp(argv[0], "echo") == 0)
        {
            // echo $PATH
            const char *target_env = NULL;
            if(argv[1][0] == '$') 
            {
                if(argv[1][1] == '?'){
                    printf("%d\n", last_exit);
                    continue;
                }
                else target_env = getenv(argv[1]+1); // "abcdefg
                
                if(target_env != NULL) printf("%s=%s\n", argv[1]+1, target_env);
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
            if (redir_type != REDIR_NONE)
            {
                // 存在文件
                // redir_type获取
                // dup2
                if (redir_type == REDIR_INPUT)
                {
                  int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                  dup2(fd, 1);
                }
                else if(redir_type == REDIR_APPEND)
                {
                  int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0666);
                  dup2(fd, 1); 
                }
                else if(redir_type == REDIR_OUTPUT)
                {
                  int fd = open(filename, O_RDONLY);
                  dup2(fd, 0);
                }
              } 

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
