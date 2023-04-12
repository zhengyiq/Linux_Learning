#pragma once

#include <iostream>
#include <vector>
#include <unistd.h>

// typedef std::function<void ()> func_t;

typedef void (*fun_t)(); //函数指针

void PrintLog()
{
    std::cout << "pid: "<< getpid() << ", 打印日志任务，正在被执行..." << std::endl;
}

void InsertMySQL()
{
    std::cout << "pid: "<< getpid() << "执行数据库任务，正在被执行..." << std::endl;
}

void NetRequest()
{
    std::cout << "pid: "<< getpid() << "执行网络请求任务，正在被执行..." << std::endl;
}

// void ExitProcess()
// {
//     exit(0);
// }

//约定，每一个command都必须是4字节
#define COMMAND_LOG 0
#define COMMAND_MYSQL 1
#define COMMAND_REQEUST 2
// #define COMMAND_QUIT 3

class Task
{
public:
    Task()
    {
        funcs.push_back(PrintLog);
        funcs.push_back(InsertMySQL);
        funcs.push_back(NetRequest);
        // funcs.push_back(ExitProcess);
    }
    void Execute(int command)
    {
        if(command >= 0 && command < funcs.size()) funcs[command]();
    }
    ~Task()
    {}
public:
    std::vector<fun_t> funcs;
};


