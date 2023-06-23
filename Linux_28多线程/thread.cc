#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <string>
#include <ctime>

#define NUM 10
using namespace std;

// 线程终止 
// 1. 函数执行完毕
// 2. pthread_exit(void*)

// void *threadRun(void* args)
// {
//     const char*name = static_cast<const char *>(args);

//     int cnt = 5;
//     while(cnt)
//     {
//         cout << name << " is running: " << cnt-- << " obtain self id: " << pthread_self() << endl;
//         if (cnt == 2) pthread_cancel(pthread_self());
//         sleep(1);
//     }

//     pthread_exit((void*)11); 

//     // PTHREAD_CANCELED; #define PTHREAD_CANCELED ((void *) -1)
// }

// int main()
// {
//     pthread_t tid;
//     pthread_create(&tid, nullptr, threadRun, (void*)"thread 1");
//     // sleep(3);

//     // pthread_cancel(tid);

//     void *ret = nullptr;
//     pthread_join(tid, &ret);
//     cout << " new thread exit : " << (int64_t)ret << "quit thread: " << tid << endl; // int有精度损失由于在64位的系统下void*有8字节而int只有四字节
//     return 0;
// }

// enum 
// {
//     OK = 0,
//     ERROR
// };


// class ThreadData
// {
// public:
//     ThreadData(const string &name, int id, time_t createTime, int top)
//     :_name(name), _id(id), _createTime((uint64_t)createTime),_status(OK), _top(top), _result(0)
//     {}
//     ~ThreadData()
//     {}
// public:
//     // 输入的
//     string _name;
//     int _id;
//     uint64_t _createTime;

//     // 返回的
//     int _status;
//     int _top;
//     int _result;
//     // char arr[n]
// };

// // class ThreadData
// // {
// // public:
// //     ThreadData(const string &name, int id, time_t createTime)
// //     :_name(name), _id(id), _createTime((uint64_t)createTime)
// //     {}
// //     ~ThreadData()
// //     {}
// // public:
// //     // 输入的
// //     string _name;
// //     int _id;
// //     uint64_t _createTime;
// // };

// class ResultData
// {};

// void *thread_run(void *args)
// {
//     ThreadData *td = static_cast<ThreadData *>(args);

//     for(int i = 1; i <= td->_top; i++)
//     {
//         td->_result += i;
//     }
//     cout << td->_name << " cal done!" << endl;
//     // pthread_exit(td);

//     return td;

//     // while (true)
//     // {
//     //     cout << "thread is running, name " << td->_name << " create time: " << td->_createTime << " index: " << td->_id << endl;
//     //     sleep(4);        
//     //     break;
//     // }
//     // delete td;
//     // pthread_exit((void*)2); 
// }

// int main()
// {
//     pthread_t tids[NUM];
//     for(int i = 0; i < NUM ;i++)
//     {
//         char tname[64];
//         snprintf(tname, 64, "thread-%d", i+1);
//         // ThreadData *td = new ThreadData(tname, i+1, time(nullptr));
//         ThreadData *td = new ThreadData(tname, i+1, time(nullptr), 100+5*i);
//         pthread_create(tids+i, nullptr, thread_run, td);
//         sleep(1);
//     }

//     void *ret = nullptr; // int a =  10

//     for(int i = 0 ; i< NUM; i++)
//     {
//         int n = pthread_join(tids[i], &ret);
//         if(n != 0) cerr << "pthread_join error" << endl;
//         // ThreadData *td = static_cast<ThreadData *>(ret);
//         // cout << "thread quit : " << (uint64_t)ret << endl;

//         ThreadData *td = static_cast<ThreadData *>(ret);
//         if(td->_status == OK)
//         {
//             cout << td->_name << " 计算的结果是: " << td->_result << " (它要计算的是[1, " << td->_top << "])" <<endl;
//         }

//         delete td;
//     }

//     cout << "all thread quit..." << endl;
//     return 0;
// }




// void* thread_run(void* args)
// {
//     char* name = (char*)args;
//     while (true)
//     {
//         cout << "new thread running, mythread name is : " << name << endl;
//         // exit(10); // 进程退出，不是线程退出
//         sleep(4);
//         break;
//     }
//     delete name;
//     pthread_exit((void*)1); // void* *ret
//     // return nullptr;
// }

// int main()
// {
//     pthread_t tid[NUM];

//     for (int i = 0; i < NUM; ++i)
//     {
//         // char tname[64];
//         char* tname = new char[64];
//         // snprintf(tname, sizeof(tname), "thread-%d", i+1); // 如果还是sizeof就只有指针大小的字节
//         snprintf(tname, 64, "thread-%d", i+1);
//         pthread_create(tid+i, nullptr, thread_run, tname);// 没有传递缓冲区，传递的是缓冲区的地址
//     }

//     void* ret = nullptr;
//     for (int i = 0; i < NUM; ++i)
//     {
//         // int n = pthread_join(tid[i], nullptr); // 阻塞式等待
//         int n = pthread_join(tid[i], &ret); // 阻塞式等待；线程异常退出就会导致主线程退出，因此不需要考虑异常
//         if (n != 0)
//         {
//             cerr << "pthread_join error" << endl;
//         }

//         cout << "thread quit: " << (uint64_t)ret << endl; // 把这个数当做整数
//     }
//     cout << "all thread quit ..." << endl;
//     return 0;


//     // sleep(3);
//     // return 0;

//     // while (true)
//     // {
//     //     cout << "main thread running" << endl;
//     //     sleep(2);
//     // }
// }

// void* thread_run(void* args)
// {
//     while (true)
//     {
//         cout << "new thread running" << endl;
//         sleep(1);
//     }

//     return nullptr;
// }

// int main()
// {
//     pthread_t t; // typedef unsigned long int pthread_t;
//     pthread_create(&t, nullptr, thread_run, nullptr);

//     while (true)
//     {
//         cout << "main thread running, newthread id : " << t << endl;
//         sleep(1);
//     }
// }