#include <iostream>
#include "./ThreadPool/Thread.hpp"
void testFunc(zwl::Thread::ThreadSharedPtr sp)
{
    for (int i = 0; i < 5; i++)
    {
        std::cout << "测试输出:" << i << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main()
{
    using namespace zwl;
    Thread::ThreadSharedPtr sp(new Thread(std::bind(testFunc, std::placeholders::_1)));
    sp->start();
    std::this_thread::sleep_for(std::chrono::seconds(6));
    return 0;
}