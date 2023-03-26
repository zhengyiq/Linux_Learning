#include "mystdio.h" 
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
// fopen("a/b/c", "a")
// fopen("a/b/c", "w")
// fopen("a/b/c", "r")
MY_FILE* my_fopen(const char* path, const char* mode)
{
    // 识别标志位
    int flag = 0;
    if (strcmp(mode, "r") == 0) flag |= O_RDONLY;
    else if(strcmp(mode, "w") == 0) flag |= (O_WRONLY | O_CREAT | O_TRUNC); 
    else if (strcmp(mode, "a") == 0) flag |= (O_WRONLY | O_CREAT | O_APPEND); 
    else{
      // other operator
      // r+ w+ a+
    }
    
    // 尝试打开文件
    mode_t m = 0666;
    int fd = 0;
    if (flag & O_CREAT) fd = open(path, flag, m);
    else fd = open(path, flag);
    
    if (fd < 0) return NULL;
    
    // 给用户返回MY_FILE对象
    MY_FILE* mf = (MY_FILE*)malloc(sizeof(MY_FILE));
    if (mf == NULL)
    {
        close(fd);
        return NULL;
    }

    // 初始化MY_FILE对象
    mf->fd = fd;
    // mf->flags = BUFF_LINE;
    mf->flags |= BUFF_LINE;
    memset(mf->outputbuffer, '\0', sizeof(mf->outputbuffer));
    // mf->outputbuffer[0] = 0
    return mf;
}

int my_fflush(MY_FILE *fp)
{
    assert(fp);
    // 将用户缓冲区中的数据，通过系统调用接口冲刷给OS
    write(fp->fd, fp->outputbuffer, fp->current);
    fp->current = 0;

    fsync(fp->fd); // 强制刷新内核缓冲区 

    return 0;
}


// 这里认为返回的就是一次实际写入的字节数，不是返回的个数
size_t my_fwrite(const void *ptr, size_t size, size_t nmemb, MY_FILE *stream)
{
    // 实际上是写入到了缓冲区中
    // 缓冲区如果已经满了则写入
    if (stream->current == NUM) my_fflush(stream);

    // 根据缓冲区剩余情况，进行数据拷贝
    size_t user_size = size * nmemb;
    size_t my_size = NUM - stream->current; // 100 - 10 = 90
    
    size_t writen = 0;

    if (my_size > user_size)
    {
        memcpy(stream->outputbuffer + stream->current, ptr, user_size);
        // 更新数据字段
        stream->current += user_size;
        writen = user_size;
    }
    else 
    {
        memcpy(stream->outputbuffer + stream->current, ptr, my_size);
        // 更新数据字段
        stream->current += my_size;
        writen = my_size;
    }
    // 开始计划刷新
    // 不发生刷新的本质，不进行写入，就是不进行IO，不进行系统调用，所以my_fwrite函数调用会非常快，数据会暂时保存在缓冲区中
    // 可以在缓冲区中积压多份数据，统一进行刷新写入，本质就是一次IO可以IO更多的数据，提高IO的效率
    if (stream->flags & BUFF_ALL)
    {
        if (stream->current == NUM) my_fflush(stream);
    }
    else if (stream->flags & BUFF_LINE)
    {
        if (stream->outputbuffer[stream->current - 1] == '\n') my_fflush(stream);
    }
    else 
    {
        // TODO
    }

    return writen;
}

int my_fclose(MY_FILE* fp)
{
    assert(fp);
    // 冲刷缓冲区
    if (fp->current > 0) my_fflush(fp);
    // 关闭文件
    close(fp->fd);
    // 释放堆空间
    free(fp);
    // 指针置空
    fp = NULL;

    return 0;
}

//int my_scanf(); stdin->buffer -> 对buffer内容进行格式化，写到对应的变量中
//int a,b; scanf("%d %d", &a, &b);read(0, stdin->buffer, num); -> 123 456 -> 输入的本质: 输入的也是字符
//扫描字符串，碰到空格，字符串分割成为两个子串，*ap = atoi(str1); *bp = atoi(str2);

//int my_printf(const char *format, ...)
//{
//    //1. 先获取对应的变量 a
//    //2. 定义缓冲区，对a转成字符串
//    //2.1 fwrite(stdout, str);
//    //3. 将字符串拷贝的stdout->buffer,即可
//    //4. 结合刷新策略显示即可
//} 
