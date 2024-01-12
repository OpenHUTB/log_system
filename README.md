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

二：
    线程池完成，多次测试调试发现，总是莫名奇妙产生动态线程，而且每次添加一个保留线程就会产生一个动态线程，最终发现是在submitTask函数里面，添加动态线程的判断逻辑出现了问题，一开始的判断逻辑是等待线程数量小于任务数量就产生动态线程，后修改逻辑为，当等待线程的数量为0时才生产动态线程，我觉得判断逻辑目前还是有一点欠妥，但是至少能解决目前绝大部分的问题，至少目前的出现动态线程的概率少了，
    还有一个问题就是发现线程池在执行完任务之后就卡在那里不动了，最后发现时在ThreadPool的析构函数里面，出现问题，应该当isQuit为false时调用stop函数，一开始写的是isQuit == true时才调用stop函数