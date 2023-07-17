#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "err.hpp"

using namespace std;

static void usage(std::string proc)
{
    std::cout << "Usage:\n\t" << proc << " serverip serverport\n" << std::endl;
}

// ./tpc_client serverip serverport
int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }

    string serverip = argv[1];
    uint16_t serverport = atoi(argv[2]);

    // 1. creatr socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        cerr << "socket error : " << strerror(errno) << endl;
        exit(SOCKET_ERR);
    }

    // 要不要bind？要
    // 要不要自己bind？不要， 因为client要让OS自动给用户进行bind
    // 要不要listen？不要 要不要accept？不需要

    // 2. connect
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(serverport);
    inet_aton(serverip.c_str(), &server.sin_addr);

    int cnt = 5;
    while (connect(sock, (struct sockaddr*)&server, sizeof(server)) != 0)
    {
        sleep(1);
        cout << "正在给你尝试重连,重连次数还有: " << cnt-- << endl;
        if(cnt <= 0) break;
    }

    if (cnt <= 0)
    {
        cerr << "连接失败..." << endl;
        exit(CONNECT_ERR);
    }

    char buffer[1024];
    // 3. 连接成功
   while(true)
    {
        string line;
        cout << "Enter>>> ";
        getline(cin, line);

        write(sock, line.c_str(), line.size());

        ssize_t s = read(sock, buffer, sizeof(buffer)-1);
        if(s > 0)
        {
            buffer[s] = 0;
            cout << "server echo >>>" << buffer << endl;
        }
        else if(s == 0)
        {
            cerr << "server quit" << endl;
            break;
        }
        else {
            cerr << "read error: " << strerror(errno) << endl;
            break;
        }
    }
    close(sock);

    return 0;
}