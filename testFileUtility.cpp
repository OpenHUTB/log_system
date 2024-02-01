#include <iostream>
#include <vector>
#include <sstream>
#include <experimental/filesystem>
//-----------------------------------------------------
#include "./ThreadPool/Thread.hpp"
#include "./ThreadPool/ThreadPoolConfig.hpp"
#include "./ThreadPool/ThreadPool.hpp"
#include "./CloudBackup/Utility.hpp"
#define delayS(s) std::this_thread::sleep_for(std::chrono::seconds(s))

//---测试文件工具类---------------
void testFileUtility()
{
    using namespace zwl;
    FileUtility fu("test01.txt");
    std::stringstream ss;
    ss << "iansdnaspdonmaposdma"<<"....................";
    fu.setContentInFile(ss.str());
    //std::cout << s << std::endl;
    // if (fu.exists())
    // {
    //     std::cout << "文件存在" << std::endl;
    // }
    // else
    // {
    //     std::cout << "文件不存在" << std::endl;
    //     std::cout << "创建文件中。。。" << std::endl;
    //     fu.create();
    //     std::cout << "文件最后一个访问时间" << fu.getLastWriteTime() << std::endl;
    // }
}
//---测试文件工具类---------------
//---测试文件压缩解压---------------
void testBundle()
{
    using namespace zwl;
    FileUtility fu("test01.lz");
    fu.uncompress("test01.txt");
}
//---测试文件压缩解压--------------
int main()
{
     testBundle();
    return 0;
}