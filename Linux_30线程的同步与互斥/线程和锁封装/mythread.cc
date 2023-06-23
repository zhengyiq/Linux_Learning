#include <iostream>
#include <cstdio>
// #include <pthread.h>
#include "thread.hpp"
#include <string>
#include <unistd.h>
#include <cstring>
#include "lockGuard.hpp"
using namespace std;
int tickets = 1000;                                // 临界资源， 加锁保证临界
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // 使用这种方式不用进行destory，若是在局部那么就需要使用pthread_mutex_init来进行初始化

void threadRoutine(void *args)
{
    string message = static_cast<const char *>(args);

    while (true)
    {
        // pthread_mutex_lock(td->_pmutex); // 所有的线程都需要遵守这个规则
        {
            LockGuard lockguard(&mutex);
            if (tickets > 0) // 临界区
            {
                usleep(2000); // 模拟抢票花费的时间，在此处进行线程的切换导致有多个进程同时处于此状态
                cout << message << " get a ticket: " << tickets-- << endl;
                // pthread_mutex_unlock(td->_pmutex);
            }
            else
            {
                // pthread_mutex_unlock(td->_pmutex);
                break;
            }
        }
        usleep(1000); // 抢完票的时候需要有后续的动作
    }
}

int main()
{
    Thread t1(1, threadRoutine, (void *)"hello 1");
    Thread t2(2, threadRoutine, (void *)"hello 2");
    Thread t3(3, threadRoutine, (void *)"hello 3");
    Thread t4(4, threadRoutine, (void *)"hello 4");

    t1.run();
    t2.run();
    t3.run();
    t4.run();

    t1.join();
    t2.join();
    t3.join();
    t4.join();
}

// void threadRun(void* args)
// {
//     string message = static_cast<const char*>(args);
//     while (true)
//     {
//         cout << "我是一个线程" << message << endl;
//         sleep(1);
//     }
// }

// int main()
// {
//     Thread t1(1, threadRun, (void*)"hello"); // 我们进行封装的时候使用的是void*类型，而这里使用的时候是const char*类型
//     cout << "thread name: " << t1.threadname() << " threadid: " << t1.threadid() << " threadstatus: " << t1.status() << endl;
//     t1.run();
//     cout << "thread name: " << t1.threadname() << " threadid: " << t1.threadid() << " threadstatus: " << t1.status() << endl;
//     t1.join();
//     cout << "thread name: " << t1.threadname() << " threadid: " << t1.threadid() << " threadstatus: " << t1.status() << endl;

//     return 0;
// }