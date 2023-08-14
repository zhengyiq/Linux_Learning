#pragma once
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "Log.hpp"
#include "Err.hpp"

static const int gbacklog = 32;
const int defaultfd = -1;

class Sock
{
public:
    Sock() : _sock(defaultfd)
    {
    }
    void Socket()
    {
        _sock = socket(AF_INET, SOCK_STREAM, 0);
        if (_sock < 0)
        {
            logMessage(Fatal, "socket error, code: %d, errstring: %s", errno, strerror(errno));
            exit(SOCKET_ERR);
        }
        // 设置地址是复用的
        int opt = 1;
        setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &opt, sizeof(opt));
    }
    void Bind(const uint16_t &port)
    {
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr = INADDR_ANY;

        if (bind(_sock, (struct sockaddr *)&local, sizeof(local)) < 0)
        {
            logMessage(Fatal, "bind error, code: %d, errstring: %s", errno, strerror(errno));
            exit(BIND_ERR);
        }
    }
    void Listen()
    {
        if (listen(_sock, gbacklog) < 0)
        {
            logMessage(Fatal, "listen error, code: %d, errstring: %s", errno, strerror(errno));
            exit(LISTEN_ERR);
        }
    }
    int Accept(std::string *clientip, uint16_t *clientport)
    {
        struct sockaddr_in temp;
        socklen_t len = sizeof(temp);
        int sock = accept(_sock, (struct sockaddr *)&temp, &len);
        if (sock < 0)
        {
            logMessage(Warning, "accept error, code: %d, errstring: %s", errno, strerror(errno));
        }
        else
        {
            *clientip = inet_ntoa(temp.sin_addr);
            *clientport = ntohs(temp.sin_port);
        }
        return sock;
    }
    int Connect(const std::string &serverip, const uint16_t &serverport)
    {
        struct sockaddr_in server;
        memset(&server, 0, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_port = htons(serverport);
        server.sin_addr.s_addr = inet_addr(serverip.c_str());

        return connect(_sock, (struct sockaddr *)&server, sizeof(server));
    }
    int Fd()
    {
        return _sock;
    }
    void Close()
    {
        if (_sock != defaultfd)
            close(_sock);
    }
    ~Sock()
    {
        
    }

private:
    int _sock;
};
