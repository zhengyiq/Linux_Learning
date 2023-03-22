
// 系统方案

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define LOG "log.txt"
int main()
{
  // fopen(LOG, "w");
  umask(0);
  // O_CREAT | O_WRONLY 默认不会对原始文件内容清空
  // int fd = open(LOG, O_CREAT | O_WRONLY, 0666);
  // int fd = open(LOG, O_CREAT | O_WRONLY | O_TRUNC, 0666);
  // int fd = open(LOG, O_WRONLY | O_CREAT | O_APPEND, 0666);
  int fd = open(LOG, O_RDONLY);
  if (fd == -1)
  {
    printf("fd:%d, errno:%d, errstring:%s\n", fd, errno, strerror(errno));
  }
  else printf("fd:%d, errno:%d, errstring:%s\n", fd, errno, strerror(errno));
  
  // 读取
  char buffer[1024];
  // 这里我们无法做到按行读取，这里是整体读取的
  ssize_t n = read(fd, buffer, sizeof(buffer) - 1); // 使用系统接口来进行I/O时，一定要注意\0的问题
  if (n > 0)
  {
    buffer[n] = '\0';
    printf("%s\n", buffer);
  }


  // // 写入 
  // const char* msg = "bbbbbbbbbbbbbbb";
  // int cnt = 1;
  // while (cnt)
  // {
  //   char line[128]; // 缓冲区
  //   snprintf(line, sizeof(line), "%s, %d\n", msg, cnt); // 格式化写入到line中

  //   // write(fd, line, strlen(line) + 1);// 这里的strlen不需要+1，\0是C语言中的规定，不是文件的规定！
  //   write(fd, line, strlen(line)); 
  //   
  //   // write(fd, msg, strlen(msg));
  //   
  //   cnt--;
  // }
  
  
  
  close(fd);
  return 0;
}




// 语言方案

// #include <stdio.h>
// #define LOG "log.txt"
// int main()
// {
  // w:默认写方式打开文件，如果文件不存在，就创建它
  // 默认只是打开，文件内容会自动清空
  // 同时每次进行写入的时候都会从最开始进行写入
  // FILE* fp = fopen(LOG, "w");
  
  // a:不会清空文件，而是每一次写入都是从文件结尾写入，追加
  // FILE* fp = fopen(LOG, "a");
  // FILE* fp = fopen(LOG, "r");
  // if (fp == NULL)
  // {
  //   perror("fopen");
  //   return 1;
  // }
  // 
  // // 正常进行文件操作
  // const char* msg = "aaa\n";
  // int cnt = 1;
  // while (cnt) 
  // {
  //   char line[128]; // 定义缓冲区
  //   if (fgets(line, sizeof(line), fp) == NULL) break;
  //   else printf("%s", line);


    // fputs(msg, fp);
   

    // char buffer[256];
    // snprintf(buffer, sizeof(buffer), "%s:%d:zyq\n", msg, cnt); // 将格式化的字符串直接放到字符串中
    // fputs(buffer, fp);
    // printf("%s", buffer);
    

    // fputs(msg, fp);
    // fprintf(fp, "%s:%d:zyq\n", msg, cnt); // 按照特定的方式向文件流中打印 
    // fprintf(stdout, "%s:%d:zyq\n", msg, cnt); // Linux一切皆文件，stdout也对应一个文件，显示器文件 // 格式化默认往显示器上打印
    // cnt--;
  // }

  // fclose(fp);
  // return 0;
// }
