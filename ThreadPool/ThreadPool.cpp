#include "ThreadPool.hpp"

namespace zwl
{
    ThreadPool::ThreadPool() : mActiveThreadNum(0), mSuspendedThreadNum(0), mShutDown(false), mTaskNum(0), mLiveThreadNum(0)
    {
        // managerThread = std::thread(std::bind(&ThreadPool::managerThreadInit, this));
    }
    ThreadPool::~ThreadPool()
    {
        if (mShutDown == false)
        {
            stop();
        }
    }
    void ThreadPool::start()
    {
        std::cout << "ThreadPool::start() 线程池开始启动" << std::endl;
        mThreadVec.reserve(MaxThreadNum);
        for (int i = 0; i < MinThreadNum; ++i)
        {
            mThreadVec.push_back(std::make_shared<std::thread>(std::bind(&ThreadPool::threadRoutine, this)));
        }
        mLiveThreadNum = mThreadVec.size();
        std::cout << "ThreadPool::start() mLiveThreadNum = " << mLiveThreadNum << std::endl;
        std::cout << "ThreadPool::start() 线程数组初始化完成" << std::endl;
    }
    void ThreadPool::stop()
    {
        std::cout << "ThreadPool::stop() 线程池开始停止" << std::endl;
        if (managerThreadSharedPtr)
            managerThreadSharedPtr->join();
        mShutDown = true;
        cond.notify_all();
        std::cout << "ThreadPool::stop() 唤醒全部线程" << std::endl;
        for (const auto &e : mThreadVec)
            if (e->joinable())
                e->join();
        std::cout << "ThreadPool::stop() 全部线程已结束" << std::endl;
        std::cout << "ThreadPool::stop() 剩余任务：" << mTaskNum << std::endl;
        mThreadVec.clear();
    }
    void ThreadPool::threadRoutine()
    {
        std::cout << "ThreadPool::threadRoutine() 线程" << std::this_thread::get_id() << "开始执行" << std::endl;
        ++mActiveThreadNum;
        std::cout << "ThreadPool::threadRoutine() 工作线程数量为：" << mActiveThreadNum << std::endl;
        while (mShutDown == false)
        {
            TaskType task = getTask();
            if (task)
            {
                task();
            }
        }
        --mActiveThreadNum;
        std::cout << "ThreadPool::threadRoutine() 线程" << std::this_thread::get_id() << "执行结束" << std::endl;
    }
    ThreadPool::TaskType ThreadPool::getTask()
    {
        TaskType task;
        if (mTaskQue.empty() == false)
        {
            std::unique_lock<std::mutex> lck(mtx);
            while (mTaskQue.empty() == true && mShutDown == false)
            {
                --mActiveThreadNum;
                ++mSuspendedThreadNum;
                cond.wait(lck);
                --mSuspendedThreadNum;
                ++mActiveThreadNum;
                // std::cout << "ThreadPool::getTask()  挂起线程数量为：" << mSuspendedThreadNum << std::endl;
            }
            // std::cout << "ThreadPool::getTask()  挂起线程数量为：" << mSuspendedThreadNum << std::endl;
            if (mTaskQue.empty() == false && mShutDown == false)
            {
                task = mTaskQue.top().second;
                mTaskQue.pop();
                --mTaskNum;
                // std::cout << "ThreadPool::getTask() 剩余任务：" << mTaskNum << std::endl;
            }
        }
        return task;
    }
    void ThreadPool::managerThreadInit()
    {
        delayS(1);
        while (mShutDown == false && mTaskNum != 0)
        {
            if (mActiveThreadNum < mTaskNum && mSuspendedThreadNum == 0 && mLiveThreadNum < MaxThreadNum)
            {
                mThreadVec.push_back(std::make_shared<std::thread>(std::bind(&ThreadPool::threadRoutine, this)));
                mLiveThreadNum = mThreadVec.size();
                // std::cout << "ThreadPool::managerThreadInit() 挂起线程数量为：" << mSuspendedThreadNum << std::endl;
                // std::cout << "ThreadPool::managerThreadInit() 当前线程数量为：" << mLiveThreadNum << std::endl;
            }
            // std::cout << "ThreadPool::managerThreadInit() 挂起线程数量为：" << mSuspendedThreadNum << std::endl;
            delayS(3);
        }
        std::cout << "ThreadPool::managerThreadInit() 当前线程数量为：" << mLiveThreadNum << std::endl;
        std::cout << "ThreadPool::managerThreadInit() 挂起线程数量为：" << mSuspendedThreadNum << std::endl;
    }
    ThreadPool *ThreadPool::getSingleton()
    {
        static ThreadPool singleton;
        return &singleton;
    }
    void ThreadPool::getManagerThreadSharedPtr()
    {
        if (managerThreadSharedPtr == nullptr)
            managerThreadSharedPtr = std::make_shared<std::thread>(std::bind(&ThreadPool::managerThreadInit, this));
    }
}
