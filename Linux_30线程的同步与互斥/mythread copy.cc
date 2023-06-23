#include <iostream>
#include <cstdio>
#include <pthread.h>
#include <string>
#include <unistd.h>
#include <cstring>

using namespace std;

int tickets = 1000; // 临界资源， 加锁保证临界
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // 使用这种方式不用进行destory，若是在局部那么就需要使用pthread_mutex_init来进行初始化
void* threadRoutine(void* name)
{
    string tname = static_cast<const char*>(name);

    while (true)
    {
        pthread_mutex_lock(&mutex); // 所有的线程都需要遵守这个规则
        if (tickets > 0) // 临界区
        {
            usleep(2000); // 模拟抢票花费的时间，在此处进行线程的切换导致有多个进程同时处于此状态
            cout << tname << " get a ticket: " << tickets-- << endl;
            pthread_mutex_unlock(&mutex); 
        }
        else
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
        usleep(1000); // 抢完票的时候需要有后续的动作
    }
    return nullptr;
}

int main()
{
    pthread_t tids[4];

    int n = sizeof(tids)/sizeof(tids[0]);

    for (int i = 0; i < n; ++i)
    {
        char* data = new char[64];
        snprintf(data, 64, "thread-%d", i + 1);
        pthread_create(tids+i, nullptr, threadRoutine, data);
    }

    for (int i = 0; i < n; ++i)
    {
        pthread_join(tids[i], nullptr);
    }
    return 0;
}