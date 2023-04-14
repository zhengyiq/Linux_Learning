#ifndef __COMM_HPP__
#define __COMM_HPP__

#include <iostream>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <string>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

// IPC_CREAT and IPC_EXCL
// 单独使用IPC_CREAT：创建一个共享内存，如果共享内存不存在，就创建，如果已经存在就获取已经存在的共享内存并返回
// IPC_EXCL不能单独使用，一般要配合IPC_CREAT
// IPC_CREAT | IPC_EXCL：创建一个共享内存，如果共享内存不存在就创建，如果已经存在，则立马出错返回 -- 如果创建成功，对应的shm，一定是最新的！

#define PATHNAME "."
#define PROJID 0x6666

const int gsize = 4096; // 暂时

key_t getKey()
{
    key_t k = ftok(PATHNAME, PROJID);
    if(k == -1)
    {
        cerr << "error: " << errno << " : " << strerror(errno) << endl;
        exit(1);
    }
    return k;
}

string toHex(int x)
{
    char buffer[64];
    snprintf(buffer, sizeof buffer, "0x%x", x);
    return buffer;
}

static int createShmHelper(key_t k, int size, int flag)
{
    int shmid = shmget(k, gsize, flag);
    if(shmid == -1)
    {
        cerr << "error: " << errno << " : " << strerror(errno) << endl;
        exit(2);
    }
    return shmid;
}

int createShm(key_t k, int size)
{
    umask(0);
    return createShmHelper(k, size, IPC_CREAT | IPC_EXCL | 0666);
}

int getShm(key_t k, int size)
{
    return createShmHelper(k, size, IPC_CREAT);
}

char* attachShm(int shmid)
{
    char *start = (char*)shmat(shmid, nullptr, 0);
    return start;
}

void detachShm(char *start)
{
    int n = shmdt(start);
    assert(n != -1);
    (void)n;
}

void delShm(int shmid)
{
    int n = shmctl(shmid, IPC_RMID, nullptr);
    assert(n != -1);
    (void)n;
}

#define SERVER 1
#define CLIENT 0

class Init
{
public:
    Init(int t):type(t)
    {
        key_t k = getKey();
        if(type == SERVER) shmid = createShm(k, gsize);
        else shmid = getShm(k, gsize);
        start = attachShm(shmid);
    }
    char *getStart(){ return start; }
    ~Init()
    {
        detachShm(start);
        if(type == SERVER) delShm(shmid);
    }
private:
    char *start;
    int type; //server or client
    int shmid;
};
#endif