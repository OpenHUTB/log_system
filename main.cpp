#include <iostream>
#include "./ThreadPool/ThreadPool.hpp"

int testFunc1(int a)
{
    return a;
}

int testFunc2(int, int)
{
    return 100;
}

int main()
{
    using namespace zwl;
    auto ptr = ThreadPool::getSingleton();
    ptr->start();
    for (int i = 0; i < 1000000; ++i)
        ptr->addTask(testFunc1, 100);
    return 0;
}