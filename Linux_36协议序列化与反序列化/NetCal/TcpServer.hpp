#pragma once

#include <iostream>
#include <pthread.h>
#include <functional>
#include "Sock.hpp"
#include "Protocol.hpp"

namespace tcpserver_ns
{
    using namespace protocol_ns;
    
    using func_t = std::function<Response (const Request&)>;
    class TcpServer;
    class ThreadData
    {
    public:
        ThreadData(int sock, std::string ip, uint16_t port, TcpServer *tsvrp)
            : _sock(sock) , _ip(ip) ,_port(port), _tsvrp(tsvrp)
        {}

        ~ThreadData()
        {}

    public:
        int _sock;
        std::string _ip;
        uint16_t _port;
        TcpServer* _tsvrp;
    };

    class TcpServer
    {
    public:
        TcpServer(func_t func, uint16_t port) : _func(func), _port(port)
        {}
        
        void InitServer()
        {
            _listensock.Socket();
            _listensock.Bind(_port);
            _listensock.Listen();
            logMessage(Info, "init server done, listensock: %d", _listensock.Fd());
        }

        static void* ThreadRoutine(void* args)
        {
            pthread_detach(pthread_self());
            ThreadData* td = static_cast<ThreadData*>(args);
            td->_tsvrp->ServiceIO(td->_sock, td->_ip, td->_port);
            logMessage(Debug, "thread running ...");
            delete td;
            return nullptr;
        }

        void Start()
        {
            for(;;)
            {
                std::string clientip;
                uint16_t clientport;
                int sock = _listensock.Accept(&clientip, &clientport);
                if (sock < 0) continue;
                logMessage(Debug, "get a new client, client info : [%s:%d]", clientip.c_str(), clientport);

                pthread_t tid;
                ThreadData *td = new ThreadData(sock, clientip, clientport, this);
                pthread_create(&tid, nullptr, ThreadRoutine, td);
            }

        } 

        // 这个函数是被多线程调用的
        void ServiceIO(int sock, const std::string &ip, const uint16_t &port)
        {
            // 1. read/recv - 如何正确的读，继续设计协议，保证用户正确的获得一个完整的报文string！
            // 你怎么保证你读到的string 就是一个完整的string风格的请求呢？？不能！

            // 我们进行一直循环读取，边读取，边检测，测试
            char buffer[1024];
            ssize_t s = recv(sock, buffer, sizeof(buffer), 0); // read

            // 2. 假设已经读到了一个完整的string
            Request req;
            req.Deserialize(str); // 对读到的request字符串要进行反序列化

            // 3. 直接提取用户的请求数据啦
            Response resp = _func(req); // 业务逻辑！

            // 4. 给用户返回响应 - 序列化
            std::string send_string;
            resp.Serialize(&send_string); // 对计算完毕的response结构要进行序列化，形成可发送字符串

            // 5. 发送到网络 -- 弱化
        }

        ~TcpServer()
        {}
    private:
        uint16_t _port;
        Sock _listensock;
        func_t _func;
    };
}