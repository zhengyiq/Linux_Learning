#include "udp_server.hpp"
#include <memory>

using namespace std;
using namespace ns_server;

// 上层的业务处理，不关心网络发送，只负责信息处理

std::string transactionString(std::string request)
{
    std::string result;
    char c;
    for(auto & r : request)
    {
        if (islower(r)) 
        {
            c = toupper(r);
            result.push_back(c);
        }
        else 
        {
            result.push_back(r);
        }
    }
    return result;
}

static bool isPass(const std::string& command)
{
    bool pass = true;
    auto pos = command.find("mv");
    if (pos != std::string::npos) pass = false;

    pos = command.find("rm");
    if (pos != std::string::npos) pass = false;

    return pass;
}

std::string execteCommand(std::string command)
{
    // FILE *popen(const char *command, const char *type);
    // 1. 创建管道
    // 2. 创建子进程
    // 3. 通过FILE*将结果直接返回，可以让用户以读取文件的访问，获得命令执行的结果

    // 安全检查
    if (!isPass(command)) return "bad";
    
    // 业务逻辑处理
    FILE* fp = popen(command.c_str(), "r");
    if (fp == nullptr) return "None";
    
    // 获取结果
    char line[2048];
    std::string result;
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        result += line;
    }
    return result;

    fclose(fp);
}

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
    // unique_ptr<UdpServer> usvr(new UdpServer(transactionString, port));
    unique_ptr<UdpServer> usvr(new UdpServer(execteCommand, port));
    // bind socket error: Cannot assign requested address 云服务器不需要bind ip地址，需要让服务器自己制定ip地址
    // 自己本地装的虚拟机或者是物理机器是允许的，

    usvr->InitServer(); //服务器的初始化
    usvr->Start();

    return 0;
}