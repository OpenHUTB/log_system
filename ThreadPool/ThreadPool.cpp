#include "ThreadPool.hpp"

namespace zwl
{
    ThreadPool::ThreadPool() : threadPoolConfiger_(ThreadPoolConfiger::getSingleton())
    {
        waitingThreadNum_.store(0);
        curThreadNum_.store(0);
        isQuit_.store(false);
    }
    ThreadPool *ThreadPool::getSingleton()
    {
        static ThreadPool singleton;
        return &singleton;
    }
    ThreadPool::~ThreadPool()
    {
        if (isQuit_ == false)
            stop();
    }

    void ThreadPool::start()
    {
        isQuit_.store(false);
        size_t reservedThreadNum = threadPoolConfiger_->DEFAULT_THREAD_NUM;
        std::cout << "初始化" << reservedThreadNum << "个保留线程" << std::endl;
        while (reservedThreadNum--)
        {
            addThread(); // 默认保留类型的线程
            // std::cout << "111" << std::endl;
        }
        // std::cout << "线程数量=" << threadList_.size() << std::endl;
    }
    void ThreadPool::stop()
    {
        isQuit_.store(true);
        UniqueLock lck(taskQueMtx_);
        notEmptyCV_.notify_all();
        exitCV_.wait(lck, [this]() -> bool
                     { return this->threadList_.size() == 0; });
        std::cout << "所有线程成功退出" << std::endl;
    }

    void ThreadPool::addThread(ThreadFlag threadFlag)
    {
        ThreadSharedPtr newThreadSharePtr(new Thread(std::bind(&ThreadPool::threadRoutine, this, std::placeholders::_1), threadFlag));
        newThreadSharePtr->start(); // 启动线程，执行threadRoutine函数
        UniqueLock lck(threadListMtx_);
        threadList_.emplace_front(newThreadSharePtr);
        ++curThreadNum_;
    }
    void ThreadPool::removeThread(ThreadId tid)
    {
        UniqueLock lck(threadListMtx_);
        for (auto it = std::begin(threadList_); it != std::end(threadList_); ++it)
        {
            if ((*it)->getThreadId() == tid)
            {
                threadList_.erase(it);
                --curThreadNum_;
                return;
            }
        }
    }

    void ThreadPool::threadRoutine(ThreadSharedPtr tsp)
    {
        while (1)
        {
            TaskType task;
            {
                UniqueLock lck(taskQueMtx_);
                ++waitingThreadNum_;
                //------------------保留线程执行的代码段-------------------
                if (tsp->getThreadFlag() == ThreadFlag::RESERVED)
                {
                    std::cout << "保留线程[" << tsp->getThreadId() << "]正在获取一个任务" << std::endl;
                    notEmptyCV_.wait(lck, [this]() -> bool
                                     { return this->isQuit_ || this->taskQue_.empty() == false; });
                    --waitingThreadNum_;
                    // 退出标志位为真，并且任务队列为空，此时才是真的退出
                    if (isQuit_ == true && taskQue_.empty() == true)
                    {
                        removeThread(tsp->getThreadId());
                        // 主线程在这个条件变量下挂起，等待任务队列为空时才会被真正唤醒
                        exitCV_.notify_all();
                        std::cout << "保留线程[" << tsp->getThreadId() << "]退出" << std::endl;
                        return;
                    }
                    else // 此时才是真的获取任务
                    {
                        task = std::move(taskQue_.front());
                        taskQue_.pop();
                        notFullCV_.notify_all();
                    }
                    //------------------保留线程执行的代码段-------------------
                }
                //---------------动态线程执行的代码段---------------------
                else
                {
                    std::cout << " 动态线程[" << tsp->getThreadId() << "]正在获取一个任务" << std::endl;
                    // 超时时间为5s，超过5s未被唤醒就会被强制唤醒
                    bool ret = notEmptyCV_.wait_for(lck, std::chrono::seconds(threadPoolConfiger_->MAX_WAIT_TIME), [this]() -> bool
                                                    { return this->isQuit_ || this->taskQue_.empty() == false; });
                    if (ret == false) // 意味着超时
                    {
                        removeThread(tsp->getThreadId());
                        // 主线程在这个条件变量下挂起，等待任务队列为空时才会被真正唤醒
                        exitCV_.notify_all();
                        std::cout << "动态线程[" << tsp->getThreadId() << "]（超时）退出" << std::endl;
                        return;
                    }
                    else if (isQuit_ == true && taskQue_.empty() == true)
                    {
                        removeThread(tsp->getThreadId());
                        // 主线程在这个条件变量下挂起，等待任务队列为空时才会被真正唤醒
                        exitCV_.notify_all();
                        std::cout << "动态线程[" << tsp->getThreadId() << "]退出" << std::endl;
                        return;
                    }
                    else
                    {
                        task = std::move(taskQue_.front());
                        taskQue_.pop();
                        notFullCV_.notify_all();
                    }
                    //---------------动态线程执行的代码段---------------------
                }
            }
            if (task != nullptr)
            {
                if (tsp->getThreadFlag() == ThreadFlag::RESERVED)
                    std::cout << "保留";
                else
                    std::cout << "动态";
                std::cout << "线程[" << tsp->getThreadId() << "]正在执行任务" << std::endl;
                task();
            }
            else
                std::cout << "任务为空" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}