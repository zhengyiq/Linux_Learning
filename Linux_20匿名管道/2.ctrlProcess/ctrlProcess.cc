#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <unistd.h>
using namespace std;

#include "Task.hpp"
const int gnum = 5;
Task t;
// 管理自己创建的管道和进程 - 先描述后组织
class EndPoint
{
public:
    pid_t _child_id;
    int _write_fd;
    EndPoint(int id, int fd):_child_id(id), _write_fd(fd)
    {}
    ~EndPoint()
    {}
};

void WaitCommand()
{   
    while (true)
    {
        int command = 0;
        int n = read(0, &command, sizeof(int));
        if (n == sizeof(int))
        {
            t.Execute(command);
        }
        else if (n == 0)
        {
            break;
        }
        else
        {
            break;
        }
    }
}

void CreatProcesses(vector<EndPoint>* end_points)
{
       // 1、先进行构建控制结构，父进程进行写入，子进程进行读取
    for (int i = 0; i < gnum; ++i)
    {
        // 1.1 创建管道
        int pipefd[2] = {0};
        int n = pipe(pipefd);
        assert(n == 0);
        (void)n;

        // 1.2 创建进程
        pid_t id = fork();
        assert(id != -1);
        
        if (id == 0)
        {
            // child
            // 1.3 关闭不需要的fd
            close(pipefd[1]); 
            // 我们期望，所有子进程得到"指令"的时候，都从标准输入中读取
            // 1.3.1 输入重定向
            dup2(pipefd[0], 0);
            // 1.3.2 子进程开始等待获取命令
            WaitCommand();

            close(pipefd[0]);
            exit(0);
        }
        else
        {
            // parent 
            // 1.3 关闭不需要的fd
            close(pipefd[0]);

            // 1.4 将新的子进程和他的管道写端构建对象
            end_points->push_back(EndPoint(id, pipefd[1]));
        }
    }

}

int main()
{
    vector<EndPoint> end_points;
    CreatProcesses(&end_points);
    // 2.得到了子进程id和对应的写端
    int num = 0;
    while(true)
    {
        //1. 选择任务
        int command = COMMAND_LOG;

        //2. 选择进程
        int index = rand()%end_points.size();

        //3. 下发任务
        write(end_points[index]._write_fd, &command, sizeof(command));

        sleep(1);
    }

    return 0;
}