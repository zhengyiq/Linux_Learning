#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#define NAME "zyq"
// int main(int argc, char* argv[], char* envp[]) 
// {
    // int cnt = 5;
    // while (cnt)
    // {
        // printf("hello %d\n", cnt--);
        // sleep(1);
    // }
    

    // char* envp[] 指针数组
    // int i = 0; 
    // for (i = 0; envp[i]; ++i)
    // {
        // printf("envp[%d]->%s\n", i, envp[i]);
    // }
   

    // environ[i] *(environ + i) 
    // extern char **environ;
    // int i = 0;
    // for(i; environ[i]; ++i)
    // {
        // printf("%s\n", environ[i]);
    // }
    
    
    // char * pwd = getenv("PWD");
    // if(pwd == NULL) 
    // {
        // perror("getenv");
    // }
    // else
    // {
        // printf("PWD：%s\n", pwd);
    // }
    
    // char* own = getenv("USER");
    // if(own == NULL) 
    // {
        // perror("getenv");
    // }
    // else
    // {
        // if (strcmp(own, NAME) == 0)
        // {
            // printf("程序允许执行...\n");
        // }
        // else
        // {
            // printf("当前用户 %s 非法\n", own);
        // }
    //}
  

    // char * env = getenv("MYENV");
		// if(env)
		// {
				// printf("myenv：%s\n", env);
		// }
    // else
    // {
        // printf("myenv：%s\n", env);
    // }
    
void Usage(const char *name)
{
    printf("\nUsage: %s -[a|b|c]\n\n", name);
    exit(0); // 终止进程
}
//int main(int argc, char *argv[], char *envp[])
// ./myproc arg
int main(int argc, char *argv[])
{
    // while(1)
    // {
        // printf(".");
        // fflush(stdout);
        // sleep(1);
    // }
    


    //if(argc != 2) Usage(argv[0]);

    // if(strcmp(argv[1], "-a") == 0) printf("打印当前目录下的文件名\n");
    // else if(strcmp(argv[1], "-b") == 0) printf("打印当前目录下的文件的详细信息\n");
    // else if(strcmp(argv[1], "-c") == 0) printf("打印当前目录下的文件名(包含隐藏文件)\n");
    // else printf("其他功能，待开发\n");

    printf("argc: %d\n", argc);
    //for(int i = 0 ; argv[i]; i++)
    for(int i = 0 ; i < argc; i++)
    {
        printf("argv[%d]->%s\n", i, argv[i]);
    }

    return 0;
}
