#include "mystdio.h"
#include <unistd.h>
#include <string.h>

#define MYFILE "log.txt"

int main()
{
    MY_FILE* fp = my_fopen(MYFILE, "w");
    if (fp == NULL) return 1; 
    
    const char *str = "hello my fwrite";
    int cnt = 5;
    // 操作文件
    while(cnt)
    {
        char buffer[1024];
        // snprintf(buffer, sizeof(buffer), "%s:%d", str, cnt--);
        snprintf(buffer, sizeof(buffer), "%s:%d\n", str, cnt--);
        size_t size = my_fwrite(buffer, strlen(buffer), 1, fp);
        sleep(1);
        printf("当前成功写入: %lu个字节\n", size);
        // if (cnt == 0)
          // sleep(10);
        //my_fflush(fp);

        // if(cnt % 5 == 0) my_fwrite("\n", strlen("\n"), 1, fp);
    }   
    my_fclose(fp);

    return 0;
}

