#include <iostream>
#include <string>
// #include <pthread.h>
#include <thread>
#include <cstring>
#include <cstdio>
#include <unistd.h>

using namespace std;

string hexAddr(pthread_t tid)
{
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "0x%x", tid);
    return buffer;
}
// 线程局部存储，定义在没给线程内部的存储
__thread int g_val = 100; // 线程的局部存储是在库中，而库是在堆栈之间因此地址会比较大，而全局变量是存储在静态区的因此地址就会比较小
// int g_val = 100;

void* threadRoutine(void* args)
{
    // pthread_detach(pthread_self()); // 一般分离放到主线程下面写
    string name = static_cast<const char*>(args);

    int cnt = 5; // 这个变量是在运行时进行开辟的 开辟空间的时候使用的是ebp - 4，因此更改ebp就能够切换不同的栈 栈的增长是ebp不变，esp向下增长
    while (cnt)
    {
        // cout << "cnt: " << cnt << "&cnt" << &cnt << endl;
        cout << name << " g_val: " << g_val++ << " &g_val: " << &g_val << endl;
        sleep(1);
    }
    return nullptr;
}

int main()
{ 
    pthread_t t1, t2, t3;
    pthread_create(&t1, nullptr, threadRoutine, (void*)"thread 1"); // 线程被创建的时候谁先执行不读确定，可能新线程被创建出来之后主线程先执行
    pthread_create(&t1, nullptr, threadRoutine, (void*)"thread 2"); 
    pthread_create(&t1, nullptr, threadRoutine, (void*)"thread 3"); 
    sleep(1); // 这里的问题就是先进行了join然后再进行detach

    while(1)
    {
        cout << "main thread id: " << hexAddr(pthread_self()) << endl;
        sleep(1);
    }

    int n1 = pthread_join(t1, nullptr); 
    int n2 = pthread_join(t2, nullptr); 
    int n3 = pthread_join(t3, nullptr); 

    // sleep(10); // 如果没有这句代码就会直接出错返回。因此一旦主线程设置为detach就不需要在进行join
    return 0;
}

// // 因为调用的是pthread的接口所以也需要进行链接
// // 语言级别的可移植性
// void run1()
// {
//     while(true)
//     {
//         cout << "thread 1" << endl;
//         sleep(1);
//     }
// }

// void run2()
// {
//     while(true)
//     {
//         cout << "thread 2" << endl;
//         sleep(1);
//     }
// }

// void run3()
// {
//     while(true)
//     {
//         cout << "thread 3" << endl;
//         sleep(1);
//     }
// }
// // C++11
// int main()
// {
//     thread th1(run1);
//     thread th2(run2);
//     thread th3(run3);

//     th1.join();
//     th2.join();
//     th3.join();
// }



// 如何理解线程库和线程ID
// 我们使用的线程库是真正存在的，那么这个库就会被加载到地址空间的共享区中，我的进程中的线程，可以随时访问库中的代码和数据。 -- 关于线程管理，比如线程的切换 -- 先描述再组织，创建类似管理线程的TCB
// 举例来说就是，我们之前在文件系统中讲过文件的管理需要struct file结构体，但是我们在语言层面上使用的是C语言提供的struct FILE和C++中的class fstream。那么在线程中也是这样，在Linux系统中使用的是LWP那么在上层进行封装的就是TCB结构，为用户提供解决方案。
// 线程库存在于共享区中，库中对于每一个线程有struct pthread、线程局部存储、线程域这些内容，然后所有的线程就如同"数组"一般的聚合在一起
// 线程ID，pthread_t就是一个地址数据！！用来标识线程相关属性集合的起始地址
// 所有新线程都有自己独立的栈结构，主线程使用的是进程系统栈，新线程使用的是库中提供的栈 线程的局部存储是在库中的

// string hexAddr(pthread_t tid)
// {
//     char buffer[64];
//     snprintf(buffer, sizeof(buffer), "0x%x", tid);
//     return buffer;
// }

// void* threadRoutine(void* args)
// {
//     // pthread_detach(pthread_self()); // 一般分离放到主线程下面写
//     string name = static_cast<const char*>(args);

//     int cnt = 5;
//     while (cnt)
//     {
//         cout << name  << " : " << hexAddr(pthread_self()) << endl;
//         sleep(1);
//     }
//     return nullptr;
// }

// int main()
// {
//     pthread_t tid;
//     pthread_create(&tid, nullptr, threadRoutine, (void*)"thread 1"); // 线程被创建的时候谁先执行不读确定，可能新线程被创建出来之后主线程先执行
//     sleep(1); // 这里的问题就是先进行了join然后再进行detach

//     while(1)
//     {
//         cout << "main thread id: " << hexAddr(pthread_self()) << "new thread id : " << hexAddr(tid) << endl;
//         sleep(1);
//     }

//     int n = pthread_join(tid, nullptr); // 

//     if (n != 0)
//     {
//         std::cerr << "error: " << n << strerror(n) << std::endl;
//     }

//     // sleep(10); // 如果没有这句代码就会直接出错返回。因此一旦主线程设置为detach就不需要在进行join
//     return 0;
// }


// 线程分离放在线程执行函数中会遇到的问题
// void* threadRoutine(void* args)
// {
//     // pthread_detach(pthread_self()); // 一般分离放到主线程下面写
//     string name = static_cast<const char*>(args);

//     int cnt = 5;
//     while (cnt)
//     {
//         cout << name  << " : " << cnt-- << endl;
//         sleep(1);
//     }
//     return nullptr;
// }

// int main()
// {
//     pthread_t tid;
//     pthread_create(&tid, nullptr, threadRoutine, (void*)"thread 1"); // 线程被创建的时候谁先执行不读确定，可能新线程被创建出来之后主线程先执行
//     pthread_detach(pthread_self()); // 一般分离放到主线程下面写
//     sleep(1); // 这里的问题就是先进行了join然后再进行detach
//     int n = pthread_join(tid, nullptr); // 

//     if (n != 0)
//     {
//         std::cerr << "error: " << n << strerror(n) << std::endl;
//     }

//     // sleep(10); // 如果没有这句代码就会直接出错返回。因此一旦主线程设置为detach就不需要在进行join
//     return 0;
// }

// // 主线程进行分离操作
// void* threadRoutine(void* args)
// {
//     string name = static_cast<const char*>(args);

//     int cnt = 5;
//     while (cnt)
//     {
//         cout << name  << " : " << cnt-- << endl;
//         sleep(1);
//     }
//     return nullptr;
// }

// int main()
// {
//     pthread_t tid;
//     pthread_create(&tid, nullptr, threadRoutine, (void*)"thread 1");
//     pthread_detach(tid);

//     int n = pthread_join(tid, nullptr); // error: 22Invalid argument

//     if (n != 0)
//     {
//         std::cerr << "error: " << n << strerror(n) << std::endl;
//     }

//     sleep(10); // 如果没有这句代码就会直接出错返回。因此一旦主线程设置为detach就不需要在进行join
//     return 0;
// }