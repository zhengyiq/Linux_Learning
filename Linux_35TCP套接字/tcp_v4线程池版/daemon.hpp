#pragma once

// 进程组 前台任务 后台任务
// 如果后台任务提到前台，老的前台任务无法运行
// 在会话中只能有一个前台任务在运行 --- 我们在命令行中启动一个进程的时候，bash就无法运行
// 如果登录就是创建一个会话，bash任务，启动我们的进程，就是在当前会话中创建新的前后台任务

// 1. setsid();
// 2. setsid(), 调用进程，不能是组长！我们怎么保证自己不是组长呢？
// 3. 守护进程a. 忽略异常信号 b. 0,1,2要做特殊处理 c. 进程的工作路径可能要更改 /

#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "log.hpp"
#include "err.hpp"

//守护进程的本质：是孤儿进程的一种！
void Daemon()
{
    // 1. 忽略信号
    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);
    // 2. 让自己不要成为组长
    if (fork() > 0) exit(0);
    // 3. 新建会话，自己成为会话的话首进程
    pid_t ret = setsid();
    if ((int)ret == -1)
    {
        logMessage(Fatal, "deamon error, code: %d, string: %s", errno, strerror(errno));
        exit(SETSID_ERR);
    }
    // 4. 可选：可以更改守护进程的工作路径
    // chdir("/")
    // 5. 处理后续的对于0,1,2的问题
    int fd = open("/dev/null", O_RDWR);
    if (fd < 0)
    {
        logMessage(Fatal, "open error, code: %d, string: %s", errno, strerror(errno));
        exit(OPEN_ERR);
    }
    dup2(fd, 0);
    dup2(fd, 1);
    dup2(fd, 2);
    close(fd);
}