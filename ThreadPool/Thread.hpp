#pragma once
#include <iostream>
#include <memory>
#include <functional>
#include <thread>
namespace zwl
{
    class Thread : public std::enable_shared_from_this<Thread>
    {
    public:
        using ThreadSharedPtr = std::shared_ptr<Thread>;
        using ThreadFunc = std::function<void(ThreadSharedPtr)>;
        enum class ThreadFlag
        {
            RESERVED,
            DYNAMIC
        };
        explicit Thread(ThreadFunc threadFunc, ThreadFlag = ThreadFlag::RESERVED);
        ~Thread() = default;
        void start();
        inline std::thread::id getThreadId()
        {
            return threadId_;
        }
        inline ThreadFlag getThreadFlag()
        {
            return threadFlag_;
        }
        void detach();
        void join();
        bool joinable();

    public:
        std::thread thread_;
        std::thread::id threadId_;
        ThreadFlag threadFlag_;
        ThreadFunc threadFunc_;
    };
}