#include <iostream>
#include <pthread.h>
#include <unistd.h>
using namespace std;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *threadRoutine(void *args)
{
    cout << "I am a new thread " << endl;

    pthread_mutex_lock(&mutex);
    cout << "I got a mutex!" << endl;

    pthread_mutex_lock(&mutex); // 申请锁的问题，它会停下来
    cout << "I alive again" << endl;

    return nullptr;
}

int main()
{
    pthread_t tid;
    pthread_create(&tid, nullptr, threadRoutine, nullptr);

    sleep(3);
    cout << "main thread run begin" << endl;
    pthread_mutex_unlock(&mutex);
    cout << "main thread unlock..." << endl;

    sleep(3);
    return 0;
}