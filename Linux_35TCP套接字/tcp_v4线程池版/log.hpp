#pragma once

#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <string>
#include <sys/types.h>
#include <unistd.h>

const std::string filename = "log/tcpserver.log";

// 日志是有日志等级的
enum
{
    Debug = 0,
    Info,
    Warning,
    Error,
    Fatal,
    Uknown
};

static std::string toLevelString(int level)
{
    switch (level)
    {
    case Debug:
        return "Debug";
    case Info:
        return "Info";
    case Warning:
        return "Warning";
    case Error:
        return "Error";
    case Fatal:
        return "Fatal";
    default:
        return "Uknown";
    }
}

static std::string getTime()
{

    // int tm_sec;			/* Seconds.	[0-60] (1 leap second) */
    // int tm_min;			/* Minutes.	[0-59] */
    // int tm_hour;			/* Hours.	[0-23] */
    // int tm_mday;			/* Day.		[1-31] */
    // int tm_mon;			/* Month.	[0-11] */
    // int tm_year;			/* Year	- 1900.  */
    // int tm_wday;			/* Day of week.	[0-6] */
    // int tm_yday;			/* Days in year.[0-365]	*/
    // int tm_isdst;			/* DST.		[-1/0/1]*/
    time_t curr = time(nullptr);
    struct tm *tmp = localtime(&curr);
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "%d-%d-%d %d:%d:%d", tmp->tm_year + 1900, tmp->tm_mon+1, tmp->tm_mday, tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
    return buffer;
}

// ...可变参数
// 日志格式：日志等级 时间 pid 消息体
// logMessage(DEBUG, "%d, %s", 12, s.c_str())
void logMessage(int level, const char* format, ...) // 传入的可能是字符串常量
{
    char logLeft[1024];

    std::string level_string = toLevelString(level);
    std::string cur_time = getTime();

    snprintf(logLeft, sizeof(logLeft), "[%s] [%s] [%d] ", level_string.c_str(), cur_time.c_str(), getpid());

    char logRight[1024];

    va_list p;
    va_start(p, format);
    vsnprintf(logRight, sizeof(logRight), format, p);
    va_end(p);

    // 打印
    // printf("%s %s\n", logLeft, logRight);

    // 保存到文件中
    FILE *fp = fopen(filename.c_str(), "a");
    if(fp == nullptr)return;
    fprintf(fp,"%s%s\n", logLeft, logRight);
    fflush(fp); //可写也可以不写
    fclose(fp);


    // va_list p; // char* 类型
    // int a = va_arg(p, int); // 根据类型提取参数
    // va_start(p, format); // 将指针指向可变参数部分的起始地址
    // va_end(p); // p = NULL
} 