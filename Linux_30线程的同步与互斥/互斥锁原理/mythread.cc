#include <iostream>
#include <cstdio>
#include <pthread.h>
#include <string>
#include <unistd.h>
#include <cstring>

using namespace std;

// 细节：
// 1. 凡是访问同一个临界资源的线程，都要进行加锁保护，而且必须加同一把锁，这是一个游戏规则，不能有例外。
// 2. 每一个线程访问临界区之前，得加锁，所以加锁是给临界区加锁，加锁的粒度尽量要细一些
// 3. 线程访问临界区的时候，需要先加锁->所有的线程都必须看到同一把锁->锁本身就是公共资源->锁如何保证自己的安全？-> 加锁和解锁本身就是原子的！
// 4. 临界区可以是一行代码，可以是一批代码， a. 线程可能被切换吗？可能，不要特殊化加锁与解锁，还有临界区代码 b. 切换会有影响吗？不会因为在我不在的期间，任何人无法进入临界区，应为它无法申请到锁，因为锁被我拿走了
// 5. 这正是体现互斥带来的串行化的表现，站在其他人的角度对其他线程有意义的状态就是：锁被我申请（持有锁），锁被我释放了（不持有锁），原子性就体现在这里
// 6. 解锁的过程也应该被设计为原子的

// 互斥锁的实现原理：互斥量
// 1. 为了实现互斥锁的操作，大多是体系结都提供了swap或exchange指令，该指令的作用就是把寄存器和内存单元的数据进行交换，由于只有一条指令，说明了加锁的原子性，且没有任何新增的1，只有1的流转->锁，钥匙
// 2. 谁在执行加锁和解锁的代码？调用线程
// 3. 寄存器只有一套但是寄存器内部的数据是每一个线程都要有的 寄存器 != 寄存器内容(执行流的上下文) 

// 因为锁申请了一个，那就说明锁一定是共享的
// lock: 
//     movb $0, %al // 调用线程，向自己的上下文中写入0
//     xchgb %al, mutex // 这里交换的本质就是将共享数据交换到自己的私有上下文中, 这里交换就是一条原子性的指令
//     if (al寄存器的内容 > 0){
//         return 0;
//     } else
//         挂起等待;
//     goto lock:
int tickets = 1000; // 临界资源， 加锁保证临界
// pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // 使用这种方式不用进行destory，若是在局部那么就需要使用pthread_mutex_init来进行初始化

class TData
{
public:
    TData(const string& name, pthread_mutex_t* mutex):_name(name), _pmutex(mutex)
    {}

    ~TData()
    {}
public:
    string _name;
    pthread_mutex_t* _pmutex;
};

void* threadRoutine(void* args)
{
    TData* td = static_cast<TData*>(args);

    while (true)
    {
        pthread_mutex_lock(td->_pmutex); // 所有的线程都需要遵守这个规则
        if (tickets > 0) // 临界区
        {
            usleep(2000); // 模拟抢票花费的时间，在此处进行线程的切换导致有多个进程同时处于此状态
            cout << td->_name << " get a ticket: " << tickets-- << endl;
            pthread_mutex_unlock(td->_pmutex); 
        }
        else
        {
            pthread_mutex_unlock(td->_pmutex);
            break;
        }
        usleep(1000); // 抢完票的时候需要有后续的动作
    }
    return nullptr;
}

int main()
{
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, nullptr);

    pthread_t tids[4];
    int n = sizeof(tids)/sizeof(tids[0]);

    for (int i = 0; i < n; ++i)
    {
        char name[64];
        snprintf(name, 64, "thread-%d", i + 1);
        TData* td = new TData(name, &mutex);

        pthread_create(tids+i, nullptr, threadRoutine, td);
    }

    for (int i = 0; i < n; ++i)
    {
        pthread_join(tids[i], nullptr);
    }

    pthread_mutex_destroy(&mutex);

    return 0;
}