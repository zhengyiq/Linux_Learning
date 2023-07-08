#include <memory>
#include "ThreadPool_V1.hpp"
int main()
{
    std::unique_ptr<ThreadPool<Task>> tp(new ThreadPool<Task>(5));
    tp->init();
    tp->start();

    // tp->check(); 

    while (true)
    {
        int x, y;
        char op;
        std::cout << "please Enter x> ";
        std::cin >> x;
        std::cout << "please Enter y> ";
        std::cin >> y;
        std::cout << "please Enter op(+-*/%)> ";
        std::cin >> op;

        Task t(x, y, op);

        tp->pushTask(t);

        // 充当生产者, 从网络中读取数据，构建成为任务，推送给线程池
        // sleep(1);
        // tp->pushTask();
    }
}