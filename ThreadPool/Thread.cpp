#include "Thread.hpp"
namespace zwl
{
    Thread::Thread(ThreadFuncType threadFunc, ThreadFlag threadFlag)
        : threadId_(0), threadFlag_(threadFlag), threadFunc_(std::move(threadFunc))
    {
    }
    void Thread::start()
    {
        thread_ = std::thread([this]() -> void
                              { 
                                this->threadId_ = std::this_thread::get_id();
                                //测试代码
                                //std::cout << "t1 = " << this->threadId_ << std::endl;
                                this->threadFunc_(this->shared_from_this()); });
        //std::cout << "t2 = " << std::this_thread::get_id() << std::endl;
        detach();
    }
    // 示例 需要用智能指针构造Thread对象，因为多次实验发现this->shared_from_this()函数底层会用对象指针来构造sharedptr，构造sharedptr需要堆区指针，如果只是使用局部对象Thread t(std::bind(testFunc, std::placeholders::_1)，在t.start()函数执行时会报错bad_weak_ptr，就是系统用局部对象的栈区指针去构造sharedptr了，对象t是局部对象，地址是栈区的，用栈区指针构造sharedptr就报错
    // using namespace zwl;
    // Thread::ThreadSharedPtr sp(new Thread(std::bind(testFunc, std::placeholders::_1)));
    // sp->start();
    // std::this_thread::sleep_for(std::chrono::seconds(6));

    void Thread::detach()
    {
        thread_.detach();
    }
    void Thread::join()
    {
        thread_.join();
    }
    bool Thread::joinable()
    {
        return thread_.joinable();
    }
}