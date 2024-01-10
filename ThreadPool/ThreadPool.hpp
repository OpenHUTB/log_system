#pragma once
#include <iostream>
#include <atomic>
#include <thread>
#include <functional>
#include <queue>
#include <memory>
#include <vector>
#include <mutex>
#include <condition_variable>
constexpr size_t MaxThreadNum = 10; // 最大线程数量
constexpr size_t MinThreadNum = 3;  // 最小线程数量
#define delayS(time) std::this_thread::sleep_for(std::chrono::seconds(time))
namespace zwl
{
    class ThreadPool
    {
    public:
        enum TaskPriorityE
        {
            level0,
            level1,
            level2
        };

        ~ThreadPool();
        void start();
        void stop();
        template <class Function, typename... Args>
        void addTask(Function &&func, Args &&...args)
        {
            TaskPairType taskPair(level2, std::bind(std::forward<Function>(func), std::forward<Args>(args)...));
            {
                std::unique_lock<std::mutex> lck(mtx);
                mTaskQue.push(taskPair);
            }
            ++mTaskNum;
            if(managerThreadSharedPtr == nullptr)
                getManagerThreadSharedPtr();
        }
        template <class Function, typename... Args>
        void addTask(TaskPriorityE level, Function &&func, Args &&...args)
        {
            TaskPairType taskPair(level, std::bind(std::forward<Function>(func), std::forward<Args>(args)...));
            {
                std::unique_lock<std::mutex> lck(mtx);
                mTaskQue.push(taskPair);
            }
            ++mTaskNum;
            if (managerThreadSharedPtr == nullptr)
                getManagerThreadSharedPtr();
        }
        static ThreadPool *getSingleton();

    private:
        struct TaskPriorityCmp;
        using TaskType = std::function<void()>;
        using TaskPairType = std::pair<TaskPriorityE, TaskType>;
        using ThreadSharedPtr = std::shared_ptr<std::thread>;
        using TaskQueType = std::priority_queue<TaskPairType, std::vector<TaskPairType>, TaskPriorityCmp>;
        struct TaskPriorityCmp
        {
            bool operator()(const TaskPairType &p1, const TaskPairType &p2)
            {
                return p1.first > p2.first;
            }
        };
        std::atomic<int> mTaskNum;                           // 任务数量
        std::atomic<int> mLiveThreadNum;                     // 当前线程的数量（包括工作线程和挂起线程）
        std::atomic<int> mActiveThreadNum;                   // 处于工作状态的线程数量
        std::atomic<int> mSuspendedThreadNum;                // 被挂起的线程数量
        std::atomic<bool> mShutDown;                         // 线程池关闭状态标志位
        std::shared_ptr<std::thread> managerThreadSharedPtr; // 管理者线程
        TaskQueType mTaskQue;                                // 任务队列
        std::vector<ThreadSharedPtr> mThreadVec;             // 线程数组
        std::mutex mtx;                                      // 互斥锁
        std::condition_variable cond;                        // 条件变量
        ThreadPool();
        void threadRoutine();
        TaskType getTask();
        void managerThreadInit();
        void getManagerThreadSharedPtr();
    };
}
