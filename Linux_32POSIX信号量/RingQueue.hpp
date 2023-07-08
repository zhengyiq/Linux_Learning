#pragma once
#include <semaphore.h>
#include <mutex>

static const int N = 5;

template <class T>
class RingQueue
{
private:
    void P(sem_t &s)
    {
        sem_wait(&s);
    }

    void V(sem_t &s)
    {
        sem_post(&s);
    }

    void Lock(pthread_mutex_t &m)
    {
        pthread_mutex_lock(&m);
    }

    void Unlock(pthread_mutex_t &m)
    {
        pthread_mutex_unlock(&m);
    }

public:
    RingQueue(int num = N) : _ring(num), _cap(num)
    {
        sem_init(&_data_sem, 0, 0);    // 消费者关心的信号量，应该被初始化为0
        sem_init(&_space_sem, 0, num); // 生产者者关心的信号量，应该被初始化为num

        _c_step = _p_step = 0;

        pthread_mutex_init(&_c_mutex, nullptr);
        pthread_mutex_init(&_p_mutex, nullptr);
    }

    // 生产
    void push(const T &in)
    {   
        // 信号量
        // 1. 可以不用在临界区内部做判断，就可以知道临界资源的使用情况
        // 2. 什么时候用锁，什么时候用sem？你对应的临界资源，是否被整体使用！
        
        P(_space_sem); // 信号量在前，锁在后 生产线程只要持有锁了，其他线程就只能在外部进行等待，本来可以先分配资源再申请锁进行生产消费
        Lock(_p_mutex);
        _ring[_p_step++] = in;
        _p_step %= _cap;
        Unlock(_p_mutex);
        V(_data_sem);

        // sem_wait(&_space_sem); // P()
        // // 一定有对应的空间资源给我! 不用做判断
        // pthread_mutex_lock(&_c_mutex);
        // _ring[_p_step++] = in;
        // _p_step %= _cap;
        // pthread_mutex_unlock(&_c_mutex);
        // sem_post(&_data_sem); // V()
    }

    // 消费
    void pop(T *out)
    {
        P(_data_sem);
        Lock(_c_mutex);
        *out = _ring[_c_step++];
        _c_step %= _cap;
        Unlock(_c_mutex);
        V(_space_sem);

        // sem_wait(&_data_sem);
        // pthread_mutex_lock(&_p_mutex);
        // *out = _ring[_c_step++];
        // _c_step %= _cap;
        // pthread_mutex_unlock(&_p_mutex);
        // sem_post(&_space_sem);
    }

    ~RingQueue()
    {
        sem_destroy(&_data_sem);
        sem_destroy(&_space_sem);

        pthread_mutex_destroy(&_c_mutex);
        pthread_mutex_destroy(&_p_mutex);
    }

private:
    vector<T> _ring;
    int _cap;         // 环形队列
    sem_t _data_sem;  // 只有消费者关心
    sem_t _space_sem; // 只有生产者关心
    int _c_step;      // 消费位置
    int _p_step;      // 生产位置

    pthread_mutex_t _c_mutex;
    pthread_mutex_t _p_mutex;
};