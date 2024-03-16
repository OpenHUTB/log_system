#include <iostream>
#include <vector>
#include <experimental/filesystem>
#include "./ThreadPool/Thread.hpp"
#include "./ThreadPool/ThreadPoolConfig.hpp"
#include "./ThreadPool/ThreadPool.hpp"
#define delayS(s) std::this_thread::sleep_for(std::chrono::seconds(s))
//--------测试Thread.cpp--------------------------------
void testFunc1(zwl::Thread::ThreadSharedPtr sp)
{
    for (int i = 0; i < 5; i++)
    {
        std::cout << "测试输出:" << i << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    // std::cout << "t2 = " << std::this_thread::get_id() << std::endl;
}
void testFunc2(zwl::Thread::ThreadSharedPtr sp)
{
    for (int i = 0; i < 5; i++)
    {
        std::cout << "测试输出:" << i << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    // std::cout << "t2 = " << std::this_thread::get_id() << std::endl;
}
void testThread()
{
    using namespace zwl;
    Thread::ThreadSharedPtr sp(new Thread(std::bind(testFunc1, std::placeholders::_1)));
    sp->start();
    Thread::ThreadSharedPtr sp1(new Thread(std::bind(testFunc2, std::placeholders::_1)));
    sp1->start();
    std::this_thread::sleep_for(std::chrono::seconds(12));
}
//--------测试Thread.cpp--------------------------------

//--------测试ThreadPool.cpp--------------------------------
int f1(int a)
{
    delayS(1);
    return a;
}
int f2(int a, int b)
{
    delayS(2);
    return a + b;
}
void testThreadPool()
{
    using namespace zwl;
    ThreadPool *tp = ThreadPool::getSingleton();
    tp->start();
    std::vector<std::shared_future<int>> vec(20);
    int i = 0;
    for (auto &e : vec)
    {
        e = tp->submitTask(f1, i++);
    }
    // std::shared_future<int> ret1 = tp->submitTask(f1, 10);
    // std::shared_future<int> ret2 = tp->submitTask(f2, 100, 20);
    // std::shared_future<int> ret3 = tp->submitTask([](int a, int b, int c) -> int
    //                                               { for (; a < b;a++)
    //                                        {a+=c;
    //                                            std::this_thread::sleep_for(std::chrono::seconds(1));
    //                                        }
    //                                        return a; },
    //                                               1, 5, 100);
    // std::cout << ret1.get() << std::endl;
    // std::cout << ret1.get() << std::endl;
    // std::cout << ret2.get() << std::endl;
    // std::cout << ret3.get() << std::endl;
    for(const auto& e: vec)
        std::cout << e.get() << std::endl;
}
//--------测试ThreadPool.cpp--------------------------------
//--------测试C++17文件系统--------------------------------
void testFileSystem()
{
    namespace filesystem = std::experimental::filesystem;
    using namespace filesystem;
    path p = "test_1.txt";
    std::cout << "当前路径为：" << filesystem::current_path() << std::endl;
    std::cout << "path[" << p << "]的绝对路径为：" << filesystem::absolute(p) << std::endl;
}
void testPath()
{
    namespace fileSystem = std::experimental::filesystem;
    using namespace fileSystem;
    fileSystem::path p;
    if(p == std::string())
    std::cout << 1 << std::endl;
}
//--------测试C++17文件系统--------------------------------

int main()
{
    // testThread();
    //testThreadPool();
    //testFileSystem();
    //testPath();
    //std::system("tree ./");
    return 0;
}