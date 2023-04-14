#include "comm.hpp"

int main()
{
    Init init(SERVER);
    char *start = init.getStart();

    int n = 0;
    while (n <= 30)
    {
        cout << "client -> server# " << start << endl;
        sleep(1);
        n++;
    }

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