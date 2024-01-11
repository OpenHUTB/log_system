#include "Thread.hpp"
namespace zwl
{
    Thread::Thread(ThreadFunc threadFunc, ThreadFlag threadFlag)
        : threadId_(0), threadFlag_(threadFlag), threadFunc_(std::move(threadFunc))
    {
    }
    void Thread::start()
    {
        thread_ = std::thread([this]() -> void
                              { this->threadId_ = std::this_thread::get_id();
                              this->threadFunc_(this->shared_from_this()); });
        detach();
    }

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