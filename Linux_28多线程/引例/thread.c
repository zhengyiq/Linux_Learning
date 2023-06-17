#include <stdio.h>
#include <pthread.h>

void* thread1_run(void* args)
{
    while(1)
    {
       printf("我是线程1, 我正在运行\n");
       sleep(1);
    }
}
void* thread2_run(void* args)
{
    while(1)
    {
       printf("我是线程2, 我正在运行\n");
       sleep(1);
    }
}
void* thread3_run(void* args)
{
    while(1)
    {
       printf("我是线程3, 我正在运行\n");
       sleep(1);
    }
}

int main()
{
    pthread_t t1, t2, t3;
    pthread_create(&t1, NULL, thread1_run, NULL);
    pthread_create(&t2, NULL, thread2_run, NULL);
    pthread_create(&t3, NULL, thread3_run, NULL);

    while(1)
    {
        printf("我是主线程，我正在运行!\n");
        sleep(1);
    }

    return 0;
}