#pragma once

class ThreadPoolConfiger
{
public:
    const unsigned int MAX_THREAD_NUM = 8;     // 最大线程数量
    const unsigned int DEFAULT_THREAD_NUM = 3; // 默认线程数量
    const unsigned int MAX_WAIT_TIME = 5;      // 在条件变量下挂起等待的最大时间（等待超时则被回收）
    const unsigned int MAX_TASK_NUM = 1000;    // 最大任务数量

    static ThreadPoolConfiger *getSingleton()
    {
        static ThreadPoolConfiger singleton;
        return &singleton;
    }
};