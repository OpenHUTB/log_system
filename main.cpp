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
#include "./CloudBackup/Server.hpp"
#include "./LogAnalysis/Analysis.hpp"
#define delayS(s) std::this_thread::sleep_for(std::chrono::seconds(s))
void run()
{
    using namespace zwl;
    std::thread hotManagerThread([]() -> void
                   { HotManager::getSingleton()->runHotManage(); });
    std::thread analyzerThread([]() -> void
                               { Analyzer::getSingleton()->startMysql(); });
    std::thread serverThread([]() -> void
                                 { Server::getSingleton()->runServer(); });
    
                               
    hotManagerThread.join();
    analyzerThread.join();
    serverThread.join();
}

int main()
{
    run();
    return 0;
}