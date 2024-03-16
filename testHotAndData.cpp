#include <iostream>
#include <vector>
#include <sstream>
#include <thread>
#include <experimental/filesystem>
//-----------------------------------------------------
#include "./ThreadPool/Thread.hpp"
#include "./ThreadPool/ThreadPoolConfig.hpp"
#include "./ThreadPool/ThreadPool.hpp"
#include "./CloudBackup/Utility.hpp"
#include "./CloudBackup/CloudBackupConfig.hpp"
#include "./CloudBackup/HotManage.hpp"
#include "./CloudBackup/DataManage.hpp"
#define delayS(s) std::this_thread::sleep_for(std::chrono::seconds(s))
//----测试Json------
void testJson()
{
    using namespace zwl;
    Json::Value root;
    std::string s;
    const char *name = "ming";
    int age{18};
    float scores[]{12.3, 43.5, 76.8};
    root["name"] = name;
    root["age"] = age;
    for (const auto &e : scores)
        root["scores"].append(e);
    JsonUtility::serialize(root, &s);
    std::cout << s << std::endl;
}
//----测试Json------
//----测试云备份配置器------
void testCloudBackupConfig()
{
    using namespace zwl;
    std::cout << CloudBackupConfiger::getSingleton()->getHotTime() << std::endl;
    std::cout << CloudBackupConfiger::getSingleton()->getServerIp() << std::endl;
    std::cout << CloudBackupConfiger::getSingleton()->getBackupInfoFilePath() << std::endl;
}
//----测试云备份配置器------
//--测试热点管理----
void testHotManage()
{
    using namespace zwl;
    HotManager::getSingleton()->RunHotManage();
}
//--测试热点管理----
//--测试数据管理----
void testDataManage()
{
    using namespace zwl;
    DataManager::getSingleton();
}
//--测试数据管理----
//--测试数据管理和热点管理----
void testHotAndData()
{
    using namespace zwl;
    std::thread t1(testDataManage);
    std::thread t2(testHotManage);
    t1.join();
    t2.join();
}
//--测试数据管理和热点管理----
int main()
{
    // testDataManage();
    //testHotAndData();
    
    return 0;
}