#pragma once

#include <iostream>
#include <pthread.h>

typedef void (*func_t)(void*);

class Thread
{
public:
    typedef enum{
        NEW = 0,
        RUNNING,
        EXITED
    }ThreadStatus;
public:
    Thread(int num, func_t func, void* args):_tid(0), _status(NEW), _func(func), _args(args)
    {
        char name[128];
        snprintf(name, sizeof(name), "thread-%d", num);
        _name = name;
    }

    int status() { return _status; };

    std::string threadname() { return _name; }

    pthread_t threadid() { 
        if (_status == RUNNING) return _tid; 
        else return 0;
    };

    static void* runHelper(void* args) // 这里的runHepler是类的成员函数，而类的成员函数是有this指针的，参数类型不匹配，因此类内的方法需要变为static，但是这样就会有新的问题：静态成员函数无法直接访问类内参数
    {
        // _func(_args);
        Thread *ts = (Thread*)args; // 这样就拿到了当前对象
        // ts->_func(ts->_args);
        (*ts)();
        return nullptr;
    }

    void operator()() // 仿函数
    {
        if (_func != nullptr) _func(_args);
    }

    void run()
    {
        int n = pthread_create(&_tid, nullptr, runHelper, this); // 为了解决上述出现的问题需要，传入this指针用来进行传参
        if (n != 0) exit(0);
        _status = RUNNING;
    } 

    void join()
    {
        int n = pthread_join(_tid, nullptr);
        if (n != 0)
        {
            std::cerr << "main thread join thread " << _name << " error" << std::endl;
            return;
        }
        _status = EXITED;
    }

    ~Thread()
    {}
private:
    pthread_t _tid;
    std::string _name;
    func_t _func; // 线程未来进行的回调
    void* _args;
    ThreadStatus _status;
};