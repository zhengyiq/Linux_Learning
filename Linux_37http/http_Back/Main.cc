#include "HttpServer.hpp"
#include "Err.hpp"
#include "Util.hpp"
#include <unordered_map>
#include <memory>
#include <vector>

std::string HandlerHttp(std::string& s)
{}

static void usage(std::string proc)
{
    std::cout << "Usage:\n\t" << proc << " port\n" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }
    uint16_t port = atoi(argv[1]);
    std::unique_ptr<HttpServer> tsvr(new HttpServer(HandlerHttp, port)); // TODO
    tsvr->InitServer();
    tsvr->Start();

    return 0;
}