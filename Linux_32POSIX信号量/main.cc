#include <iostream>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <vector>
#include <cstring>

using namespace std;

#include "Task.hpp"
#include "RingQueue.hpp"

const char *ops = "+-*/%";

void* consumerRoutine(void* args)
{
    RingQueue<Task> *rq = static_cast<RingQueue<Task>*>(args);
    while (true)
    {
        // sleep(1);
        Task t;
        rq->pop(&t);
        t();
        cout << pthread_self() << " " << "consumer done, 处理完成的任务是：" << t.formatRes() << endl;
    }
}

void* productorRoutine(void* args)
{
    RingQueue<Task> *rq = static_cast<RingQueue<Task>*>(args);
    while (true)
    {
        int x = rand() % 100;
        int y = rand() % 100;
        char op = ops[(x + y) % strlen(ops)];
        Task t(x, y, op);
        rq->push(t);
        cout << pthread_self() << " " << "productor done, 生产的任务是: " << t.formatArg() << endl;
        sleep(1);
    }
}

int main()
{
    srand(time(nullptr) ^ getpid());
    RingQueue<Task> *rq = new RingQueue<Task>();

    // 建立正确的认知，不一定多生产多消费效率就高
    // 多生产，多消费，该如何更改代码呢？done
    // 意义在哪里呢？意义绝对不在从缓冲区冲放入和拿去，意义在于，放前并发构建Task，获取后多线程可以并发处理task，因为这些操作没有加锁！
    pthread_t c[3], p[2];
    for (int i = 0; i < 3; i++)
        pthread_create(c + i, nullptr, consumerRoutine, rq);
    for (int i = 0; i < 2; i++)
        pthread_create(p + i, nullptr, productorRoutine, rq);

    for (int i = 0; i < 3; i++)

        pthread_join(c[i], nullptr);
    for (int i = 0; i < 2; i++)

        pthread_join(p[i], nullptr);

    // // 单生产单消费
    // pthread_t c, p;
    // pthread_create(&c, nullptr, consumerRoutine, rq);
    // pthread_create(&p, nullptr, productorRoutine, rq);

    // pthread_join(c, nullptr);
    // pthread_join(p, nullptr);

    delete rq;

    return 0;
}

// void* consumerRoutine(void* args)
// {
//     RingQueue<int> *rq = static_cast<RingQueue<int>*>(args);
//     while (true)
//     {
//         int data;
//         rq->pop(&data);
//         cout << "consumer done:" << data << endl;

//     }
// }

// void* productorRoutine(void* args)
// {
//     RingQueue<int> *rq = static_cast<RingQueue<int>*>(args);
//     while (true)
//     {
//         int data = rand() % 10 + 1;
//         rq->push(data);
//         cout << "productor done:" << data << endl;
//         sleep(1);
//     }
// }

// int main()
// {
//     srand(time(nullptr) ^ getpid());
//     RingQueue<int> *rq = new RingQueue<int>();

//     // 单生产单消费
//     pthread_t c, p;
//     pthread_create(&c, nullptr, consumerRoutine, rq);
//     pthread_create(&p, nullptr, productorRoutine, rq);

//     pthread_join(c, nullptr);
//     pthread_join(p, nullptr);

//     delete rq;

//     return 0;
// }