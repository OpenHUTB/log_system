#pragma once
namespace zwl
{
    class ThreadPoolConfiger
    {
    public:
        const unsigned int MAX_THREAD_NUM = 8;     // 最大线程数量
        const unsigned int DEFAULT_THREAD_NUM =3; // 默认线程数量
        const unsigned int MAX_WAIT_TIME = 5;      // 在条件变量下挂起等待的最大时间（等待超时则被回收）
        const unsigned int MAX_TASK_NUM = 1000;    // 最大任务数量

        static ThreadPoolConfiger *getSingleton()
        {
            static ThreadPoolConfiger singleton;
            return &singleton;
        }
        ~ThreadPoolConfiger() {} // 析构函数要public，不然静态单例无法在类外部销毁
    private:
        ThreadPoolConfiger() {}                                  // 单例模式构造函数private
        ThreadPoolConfiger(const ThreadPoolConfiger &) = delete; // 单例模式禁止拷贝构造和重载=运算符
        ThreadPoolConfiger &operator=(const ThreadPoolConfiger &) = delete;
    };
}