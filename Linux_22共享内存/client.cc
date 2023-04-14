#include "comm.hpp"

int main()
{
    Init init(CLIENT);
    char *start = init.getStart();
    char c = 'A';

    while(c <= 'Z')
    {
        start[c-'A'] = c;
        c++;
        start[c-'A'] = '\0';
        sleep(1);
    }

    // key_t k = getKey();
    // cout << "server key: " << toHex(k) << endl;

    // int shmid = getShm(k, gsize);
    // cout << "client shmid: " << shmid << endl;

    // char* start = attachShm(shmid);

    // sleep(3);
    // detachShm(start);
    // return 0;
}