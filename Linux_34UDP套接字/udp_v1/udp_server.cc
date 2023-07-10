#include "udp_server.hpp"
#include <memory>

using namespace std;
using namespace ns_server;
int main()
{
    unique_ptr<UdpServer> usvr(new UdpServer("1.1.1.1", 8082));

    usvr->InitServer(); //服务器的初始化
    usvr->Start();

    return 0;
}