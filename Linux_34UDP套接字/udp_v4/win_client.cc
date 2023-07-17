#include <iostream>
#include <string>
#include <WinSock2.h>

#pragma warning(disable:4996)

#pragma comment(lib, "ws2_32.lib")

uint16_t serverport = 18989;
std::string serverip = "120.26.50.214";

// easyx ege
int main()
{
    WSADATA WSAData;

    if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) // 检查库的版本是否一致
    {
        std::cerr << "init error" << std::endl;
        return -1;
    }

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        std::cerr << "create socket error" << std::endl;
        exit(-2);
    }
    // client 这里要不要bind呢？要的！socket通信的本质[clientip:clientport, serverip:serverport]
    // 要不要自己bind呢？不需要自己bind，也不要自己bind，OS自动给我们进行bind -- 为什么？client的port要随机让OS分配防止client出现
    // 启动冲突 -- server 为什么要自己bind？1. server的端口不能随意改变，众所周知且不能随意改变的 2. 同一家公司的port号
    // 需要统一规范化

    // 明确server是谁
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(serverport);
    server.sin_addr.s_addr = inet_addr(serverip.c_str());

    while (true)
    {
        // 用户输入
        std::string message;
        std::cout << "Please Enter Your Message# "; // 2
        //std::cin >> message;

        std::getline(std::cin, message);
        // 什么时候bind的？在我们首次系统调用发送数据的时候，OS会在底层随机选择clientport+自己的IP,1. bind 2. 构建发送的数据报文
        // 发送
        sendto(sock, message.c_str(), message.size(), 0, (struct sockaddr*)&server, sizeof(server));


        // 接受
        char buffer[2048];
        struct sockaddr_in temp;
        int len = sizeof(temp);
        int n = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&temp, &len);
        if (n > 0)
        {
            buffer[n] = 0;
            std::cout << buffer << std::endl; //1
        }
    }

    closesocket(sock);
    WSACleanup();

    return 0;
}