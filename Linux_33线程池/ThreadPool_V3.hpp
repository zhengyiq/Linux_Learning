#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unistd.h>
#include "Thread.hpp"
#include "Task.hpp"
#include "lockGuard.hpp"
const static int N = 5;

template <class T>
class ThreadPool
{
public:
    ThreadPool(int num = N) : _num(num)
    {
        pthread_mutex_init(&_lock, nullptr);
        pthread_cond_init(&_cond, nullptr);
    }
   
    void threadWait()
    {
        pthread_cond_wait(&_cond, &_lock);
    }

    void threadWakeup()
    {
        pthread_cond_signal(&_cond);
    }

    bool isEmpty()
    {
        return _tasks.empty();
    }

    pthread_mutex_t* getLock()
    {
        return &_lock;
    }

    T popTask()
    {
        T t = _tasks.front();
        _tasks.pop();
        return t;
    }
    static void threadRoutine(void *args)
    {
        // pthread_detach(pthread_self());
        ThreadPool<T> *tp = static_cast<ThreadPool<T> *>(args);
        while (true)
        {
            // 1. 检测有没有任务
            // 2. 有：处理
            // 3. 无：等待
            // 细节：必定加锁
            T t;
            { // 能够进行自动的析构
                LockGuard lockguard(tp->getLock()); // 填入的是锁的地址
                while (tp->isEmpty())
                {
                    tp->threadWait();
                }
                t = tp->popTask(); // 从公共区域拿到私有区域
            }
            // for test
            t();
            std::cout << "thread handler done, result: " << t.formatRes() << std::endl;
            // t.run(); // 处理任务，应不应该在临界区中处理？1,0
        }
    }
    void init()
    {
        for (int i = 0; i < _num; i++)
        {
            _threads.push_back(Thread(i, threadRoutine, this));
        }
    }
    void start()
    {
        for (auto &t : _threads)
        {
            t.run();
        }
    }
    void check()
    {
        for (auto &t : _threads)
        {
            std::cout << t.threadname() << " running..." << std::endl;
        }
    }
    void pushTask(const T &t)
    {
        LockGuard lockguard(&_lock);
        _tasks.push(t);
        threadWakeup();
    }
    ~ThreadPool()
    {
        for (auto &t : _threads)
        {
            t.join();
        }
        pthread_mutex_destroy(&_lock);
        pthread_cond_destroy(&_cond);
    }

private:
    std::vector<Thread> _threads;
    int _num;

    std::queue<T> _tasks; // 使用stl的自动扩容的特性

    pthread_mutex_t _lock;
    pthread_cond_t _cond;
};