#include "udp_client.hpp"

// 127.0.0.1 本地回环，表示的就是当前的主机，通常用来进行本地通信或者测试

static void usage(std::string proc)
{
    std::cout << "Usage:\n\t" << proc << " serverip serverport\n"
              << std::endl;
}

// ./udp_client serverip serverport
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }

    std::string serverip = argv[1];
    uint16_t serverport = atoi(argv[2]);

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        std::cerr << "create socket error" << std::endl;
        exit(SOCKET_ERR);
    }

    // client 这里要不要bind？ 要的！socket通信的本质[clientip:clientport, serverclient. serverport]
    // 要不要自己bind？不需要自己bind，os自动给我们进行bind！-- 为什么？client的port要随机让os分配防止client出现启动冲突
    // server为什么要自己bind -- 1. server的端口不能随意改变, 众所周知且不能随意改变；2. 同一家公司的port需要统一进行管理

    // 明确server是谁？
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(serverport);
    server.sin_addr.s_addr = inet_addr(serverip.c_str());

    while (true)
    {
        // 用户输入
        std::string message;
        std::cout << "please Enter# ";
        std::cin >> message;

        // 什么时候bind？在我们首次系统调用发送数据的时候，OS会在底层随机选择clientport + 自己的IP 1. bind 2. 构建发送的数据报文


        // 发送
        sendto(sock, message.c_str(), message.size(), 0, (struct sockaddr *)&server, sizeof(server));

        // 接收
        char buffer[2048];
        struct sockaddr_in temp;
        socklen_t len = sizeof(temp);
        int n = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&temp, &len);
        if (n > 0)
        {
            buffer[n] = 0;
            std::cout << "server echo# " << buffer << std::endl;
        }
    }

    return 0;
}