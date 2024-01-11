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