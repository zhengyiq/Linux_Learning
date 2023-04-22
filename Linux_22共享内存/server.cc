#include "comm.hpp"

int main()
{
    Init init(SERVER);

    // start指向了共享内存的其实空间
    char *start = init.getStart();

    int n = 0;

    // 在通信的时候，没有使用任何的接口？一旦共享内存映射到进程的地址空间，该共享内存就直接被所有的进程直接看到了
    // 因为共享内存的这种特性，可以让进程通信的时候减少拷贝次数，所以共享内存是所有进程间通信速度最快的。
    // 共享内存没有任何的保护机制(同步互斥) -- 为什么? 管道是通过系统接口通信的，共享内存直接通信
    while (n <= 30)
    {
        cout << "client -> server# " << start << endl;
        sleep(1);
        n++;
    }

    // 扩展内容：
    // 1. client写完了，才通知server读取，刚开始一定先让client运行
    // 2. 命名管道
    // 3. client写完了，才通知server读取，读取完了才让client进行写入 两个管道
    // 

    // // 1. 创建key
    // key_t k = getKey();
    // cout << "server key: " << toHex(k) << endl;

    // // 2. 创建共享内存
    // int shmid = createShm(k, gsize);
    // cout << "server shmid: " << shmid << endl;

    // sleep(5);
    // // 3. 将自己和共享内存关联起来
    // char* start = attachShm(shmid);

    // // 通信代码

    // sleep(5);
    // // 4. 将自己和共享内存去关联
    // detachShm(start);
    // sleep(5);
    // // 5. 删除共享内存
    // delShm(shmid);

    // struct shmid_ds ds;
    // int n = shmctl(shmid, IPC_STAT, &ds);
    // if(n != -1)
    // {
    //     cout << "key: " << toHex(ds.shm_perm.__key) << endl;
    //     cout << "creater pid: " << ds.shm_cpid  << " : " << getpid() << endl;
    // }

    return 0;
}