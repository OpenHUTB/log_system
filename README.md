注意一：
需要用智能指针构造Thread对象，因为多次实验发现this->shared_from_this()函数底层会用对象指针来构造sharedptr，构造sharedptr需要堆区指针，如果只是使用局部对象Thread t(std::bind(testFunc, std::placeholders::_1)，在t.start()函数执行时会报错bad_weak_ptr，就是系统用局部对象的栈区指针去构造sharedptr了，对象t是局部对象，地址是栈区的，用栈区指针构造sharedptr就报错。
------------正确-------------------------
    using namespace zwl;
    Thread::ThreadSharedPtr sp(new Thread(std::bind(testFunc, std::placeholders::_1)));
    sp->start();
    std::this_thread::sleep_for(std::chrono::seconds(6));
------------正确-------------------------
------------错误-------------------------
 Thread t(std::bind(testFunc, std::placeholders::_1)));
 t.start();
------------错误-------------------------

注意二：
// 最好不要写当waitingThreadNum_ == 0 的时候添加线程，因为多次实验发现，在一些特殊情况下，5个线程执行10万个任务，理论上不会出现挂起的线程，首先我在实验时waitingThreadNum_一直都是原子类型，不存在不安全的问题，将要进入uniquelock的wait时，waitingThreadNum_++，结束wait函数后waitingThreadNum_--，如果使用waitingThreadNum_ == 0作为判断添加线程的条件的话，条件过于苛刻，线程执行速度非常快，waitingThreadNum_的数值一直在非常快速的变化，很难在判断条件的这一时刻数值变为0，
    if(waitingThreadNum_<taskNum&&curThreadNum_<threadPoolConfiger_->MAX_THREAD_NUM)
    {
        addThread(ThreadFlag::DYNAMIC);
    }