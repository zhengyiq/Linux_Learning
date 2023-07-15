#include "udp_server.hpp"
#include <memory>

using namespace std;
using namespace ns_server;


static void usage(string proc)
{
    std::cout << "Usage:\n\t" << proc << " port\n" << std::endl;
}

// ./udp_server port
int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }

    uint16_t port = atoi(argv[1]); // 此处为char*类型的数据若是要传入port需要进行转换
    // unique_ptr<UdpServer> usvr(new UdpServer("1.1.1.1", 8082));
    unique_ptr<UdpServer> usvr(new UdpServer(port));
    // bind socket error: Cannot assign requested address 云服务器不需要bind ip地址，需要让服务器自己制定ip地址
    // 自己本地装的虚拟机或者是物理机器是允许的，

    usvr->InitServer(); //服务器的初始化
    usvr->Start();

    return 0;
}