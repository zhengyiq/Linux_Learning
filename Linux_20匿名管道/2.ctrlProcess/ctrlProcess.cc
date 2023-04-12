#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;

#include "Task.hpp"
const int gnum = 3;
Task t;
// 管理自己创建的管道和进程 - 先描述后组织
class EndPoint
{
    static int number;

public:
    pid_t _child_id;
    int _write_fd;
    std::string processname;

public:
    EndPoint(int id, int fd):_child_id(id), _write_fd(fd)
    {
        // process-0[pid:fd]
        char namebuffer[64];
        snprintf(namebuffer, sizeof(namebuffer), "process-%d[%d:%d]", number++, _child_id, _write_fd);
        processname = namebuffer;
    }

    // 获取构建好的进程名
    const std::string& name() const
    {
        return processname;
    }

    ~EndPoint()
    {}
};
int EndPoint::number = 0;

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
            std::cout << "父进程让我退出，我就退出了" << getpid() << std::endl;
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
    vector<int> fds;
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
            // for (auto &fd : fds) close(fd);
            // 为了解决退出时同一个管道的写端有子进程还有父进程同时指向的问题
            std::cout << getpid() << "子进程关闭附近父进程对应的写端:";
            for (auto &fd : fds) 
            {
                std::cout << fd << " ";
                close(fd);
            }
            std::cout << std::endl;

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
            fds.push_back(pipefd[1]);
        }
    }

}

int showBoard()
{
    std::cout << "#####################################" << std::endl;
    std::cout << "#1.执行日志任务 2.执行数据库任务     #" << std::endl;
    std::cout << "#2.执行请求任务 3.退出               #" << std::endl;
    std::cout << "#####################################" << std::endl;
    std::cout << "请选择#";
    int command = 0;
    std::cin >> command;
    return command;
}

void CtrlProcesses(const vector<EndPoint>& end_points)
{
    int num = 0;
    int cnt = 0;
    while (true)
    {
        // 1. 选择任务
        int command = showBoard();
        if (command == 3) break;
        if (command < 0 || command > 4) continue;
        // 2. 选择进程
        int index = cnt++;
        cnt %= end_points.size();
        // if (command == 3) 
        // {
        //     std::cout << "选择结束该子进程" << std::endl;
        //     write(end_points[index]._write_fd, &command, sizeof(command));
        //     continue;
        // }
        std::cout << "选择了进程" << end_points[index].name() << "|处理任务" << command << std::endl;

        // 3. 下发任务
        write(end_points[index]._write_fd, &command, sizeof(command));

        sleep(1);
    }
}


void WaitProcesses(const vector<EndPoint>& end_points)
{
    // 倒着退出可以正常进行退出
    for (int end = 0; end <= end_points.size()-1; ++end)
    {
        std::cout << "父进程让子进程退出:" << end_points[end]._child_id << std::endl;
        close(end_points[end]._write_fd);
        std::cout << "父进程回收了子进程" << end_points[end]._child_id << std::endl;
        // 在父进程关闭写端的时候管道的写端并没有完全关闭，因此子进程不会退出，会卡住
        waitpid(end_points[end]._child_id, nullptr, 0);
    }
} 

// void WaitProcesses(const vector<EndPoint>& end_points)
// {
//     // 倒着退出可以正常进行退出
//     for (int end = end_points.size()-1; end >= 0; --end)
//     {
//         std::cout << "父进程让子进程退出:" << end_points[end]._child_id << std::endl;
//         close(end_points[end]._write_fd);
//         std::cout << "父进程回收了子进程" << end_points[end]._child_id << std::endl;
//         // 在父进程关闭写端的时候管道的写端并没有完全关闭，因此子进程不会退出，会卡住
//         waitpid(end_points[end]._child_id, nullptr, 0);
//     }
// } 

// void WaitProcesses(const vector<EndPoint>& end_points)
// {
//     // 这里的问题是在循环建立子进程的时候，后面创建的子进程会继承前面开辟的管道的文件数组信息，因此管道的输入会有多个子进程文件指向该管道
//     for (const auto &ep : end_points)
//     {
//         std::cout << "父进程让子进程退出:" << ep._child_id << std::endl;
//         close(ep._write_fd);
//         std::cout << "父进程回收了子进程" << ep._child_id << std::endl;
//         // 在父进程关闭写端的时候管道的写端并没有完全关闭，因此子进程不会退出，会卡住
//         waitpid(ep._child_id, nullptr, 0);
//     }
// } 

// void WaitProcesses(const vector<EndPoint>& end_points)
// {
//     // 这里编写了两个循环
//     // 1. 需要让子进程全部退出 --- 只需要让父进程关闭所有的写端 _write_fd就可以了
//     for (const auto &ep : end_points)
//     {
//         close(ep._write_fd);
//     }
//     std::cout << "父进程让所有的子进程全部退出" << std::endl;
//     sleep(2);

//     // 2. 父进程需要回收子进程的僵尸状态 -- waitpid
//     for (const auto &ep : end_points)
//     {
//         waitpid(ep._child_id, nullptr, 0);
//     }
//     std::cout << "父进程回收了所有的子进程" << std::endl;
//     sleep(1);
// }


int main()
{
    vector<EndPoint> end_points;
    // 1. 先进行构建控制结构，父进程写入，子进程读取 bug？
    CreatProcesses(&end_points); 

    // 2. 得到了子进程id和对应的写端
    // 2.1 可以写成自动化的，也可以做成交互式的
    CtrlProcesses(end_points);

    // 3. 处理所有的退出问题
    WaitProcesses(end_points);

    return 0;
}

// int main()
// {
//     vector<EndPoint> end_points;
//     CreatProcesses(&end_points);
//     // 2.得到了子进程id和对应的写端
//     // 2.1 可以写成自动化的，也可以做成交互式的
//     int num = 0;
//     while(true)
//     {
//         //1. 选择任务
//         int command = COMMAND_LOG;

//         //2. 选择进程
//         int index = rand()%end_points.size();

//         //3. 下发任务
//         write(end_points[index]._write_fd, &command, sizeof(command));

//         sleep(1);
//     }

//     return 0;
// }