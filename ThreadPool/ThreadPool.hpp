#pragma once
#include <iostream>
#include <functional>
#include <thread>
#include <memory>
#include <future>
#include <list>
#include <mutex>
#include <queue>
#include <atomic>
#include <condition_variable>
//---------------------------------------
#include "ThreadPoolConfig.hpp"
#include "Thread.hpp"
namespace zwl
{
    class ThreadPool
    {
    public:
        using TaskType = std::function<void()>;
        using ThreadSharedPtr = Thread::ThreadSharedPtr;
        using ThreadFlag = Thread::ThreadFlag;
        using UniqueLock = std::unique_lock<std::mutex>;
        using CondVari = std::condition_variable;
        using ThreadId = std::thread::id;
        //---------------------------------------------------------
        static ThreadPool *getSingleton(); // 获取单例指针
        ~ThreadPool();
        void start();
        void stop();
        template <class Func, typename... Args>
        auto submitTask(Func &&func, Args &&...args) -> std::future<decltype(func(args...))>;

    private:
        explicit ThreadPool();                   // 显式构造，不允许隐式类型转换
        ThreadPool(const ThreadPool &) = delete; // 单例模式禁止拷贝构造和赋值重载
        ThreadPool &operator=(const ThreadPool &) = delete;

        void addThread(ThreadFlag threadFlag = ThreadFlag::RESERVED); // 添加线程（保留线程和动态线程）
        void threadRoutine(ThreadSharedPtr);                          // 线程执行函数
        void removeThread(ThreadId);                                  // 移除线程

        std::list<ThreadSharedPtr> threadList_;  // 线程链表（线程数组），管理线程池中的线程
        std::queue<TaskType> taskQue_;           // 任务队列
        ThreadPoolConfiger *threadPoolConfiger_; // 线程池配置器指针
        std::mutex threadListMtx_;               // 线程链表互斥锁
        std::mutex taskQueMtx_;                  // 任务队列互斥锁
        std::atomic<int> waitingThreadNum_;      // 挂起等待的线程数量
        std::atomic<int> curThreadNum_;          // 当前线程数量
        std::atomic<bool> isQuit_;               // 线程池退出标志位
        CondVari notFullCV_;                     // 任务队列不为满
        CondVari notEmptyCV_;                    // 任务队列不为空
        CondVari exitCV_;                        // 没有任务是退出
    };
    template <class Func, typename... Args>
    auto ThreadPool::submitTask(Func &&func, Args &&...args) -> std::future<decltype(func(args...))>
    {
        using ReturnType = decltype(func(args...));
        auto task = std::make_shared<std::packaged_task<ReturnType()>>(std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
        std::future<ReturnType> res = task->get_future();
        size_t taskNum = 0;
        {
            UniqueLock lck(taskQueMtx_);
            notFullCV_.wait(lck, [this]() -> bool
                            { bool notFull = false;
                if (taskQue_.size() < threadPoolConfiger_->MAX_TASK_NUM) notFull = true;
                else std::cout<<"任务队列已满"<<std::endl; 
                return notFull; });
            taskQue_.emplace([task]() -> void
                             { (*task)(); });
            taskNum = taskQue_.size();
            notEmptyCV_.notify_one();
        }

        // if (waitingThreadNum_ < taskNum && curThreadNum_ < threadPoolConfiger_->MAX_THREAD_NUM)
        // {
        //     addThread(ThreadFlag::DYNAMIC);
        // }
        if (waitingThreadNum_ == 0 && curThreadNum_ < threadPoolConfiger_->MAX_THREAD_NUM)
        {
            addThread(ThreadFlag::DYNAMIC);
        }
        return res;
    }
}