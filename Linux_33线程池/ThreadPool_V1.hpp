#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unistd.h>
#include <pthread.h>
#include "Task.hpp"

const static int N = 5;

template<class T>
class ThreadPool
{
public:
    ThreadPool(int num = N) : _num(num), _threads(num)
    {
        pthread_mutex_init(&_lock, nullptr);
        pthread_cond_init(&_cond, nullptr);
    }

    void lockQueue()
    {
        pthread_mutex_lock(&_lock);
    }

    void unlockQueue()
    {
        pthread_mutex_unlock(&_lock);
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

    T popTask()
    {
        T t = _tasks.front();
        _tasks.pop();
        return t;
    }

    static void* threadRoutine(void* args) // 静态的方法无法使用类内的成员
    {
        // 1. 检测有没有任务
        // 2. 有：处理
        // 3. 无：等待
        // 细节：必定加锁
        pthread_detach(pthread_self());
        ThreadPool<T> *tp = static_cast<ThreadPool<T>*>(args);
        while (true)
        {
            tp->lockQueue();
            while (tp->isEmpty())
            {
                // 等待，cond
                tp->threadWait();
            }
       
        T t = tp->popTask(); // 将数据从公共区域拿到私有区域
        tp->unlockQueue();
        
        t(); // 处理任务不应该在临界区中
        std::cout << "thread handler done, result: " << t.formatRes() << std::endl;
        // // t.run();
        
        // while (1)
        // {
        //     sleep(1);
        //     std::cout << pthread_self() << " running ..." << std::endl;
        // }
        }
    }

    void init()
    {
        // TODO
    }

    void start()
    {
        for (int i = 0; i < _num; i++)
        {
            pthread_create(&_threads[i], nullptr, threadRoutine, this);
        }
    }

    void pushTask(const T& in)
    {
        lockQueue();
        _tasks.push(in);
        threadWakeup();
        unlockQueue();
    } 
    
    ~ThreadPool()
    {
        pthread_mutex_destroy(&_lock);
        pthread_cond_destroy(&_cond);
    }

private:
    std::vector<pthread_t> _threads;
    int _num;

    std::queue<T> _tasks; // 使用stl的自动扩容的特性

    pthread_mutex_t _lock;
    pthread_cond_t _cond;
};