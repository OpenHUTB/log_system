#include <vector>
#include <thread>
#include "HotManage.hpp"
#include "Utility.hpp"
#include "CloudBackupConfig.hpp"
#include "DataManage.hpp"
namespace zwl
{
    HotManager::HotManager()
    {
        CloudBackupConfiger *configer = CloudBackupConfiger::getSingleton();
        hotTime_ = configer->getHotTime();
        backDirPath_ = configer->getBackDirPath();
        packDirPath_ = configer->getPackDirPath();
        packFileSuffix_ = configer->getPackFileSuffix();
        FileUtility fu1(backDirPath_);
        if (fu1.exists() == false)
            fu1.create();
        FileUtility fu2(packDirPath_);
        if (fu2.exists() == false)
            fu2.create();
    }
    HotManager *HotManager::getSingleton()
    {
        static HotManager singleton;
        return &singleton;
    }

    void HotManager::runHotManage()
    {
        FileUtility fu(backDirPath_);
        DataManager *dataManager = DataManager::getSingleton();
        while (1)
        {
            std::vector<std::string> vec;
            fu.scanDirectory(&vec);
            for (const auto &e : vec)
            {
                BackInfo info;
                dataManager->getBackInfoByBackPathFromTable(e, &info);
                if (info.backPath_ == std::string())
                {
                    std::cout << e << "文件存在但无备份信息" << std::endl;
                    info.createBackInfo(e);
                    dataManager->insert(info);
                }
                else
                {
                    info.fileSize_ = FileUtility(e).getFileSize();
                    dataManager->update(info);
                }
                if(info.loopTime_++ < 10)
                    continue;
                if (isHotFile(e))
                    continue;
                FileUtility fu(e);
                fu.compress(info.packPath_);
                fu.remove();
                info.isPacked_ = true;
                dataManager->update(info);
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    }
    bool HotManager::isHotFile(const std::string &backPath)
    {
        FileUtility fu(backPath);
        if (fu.exists() == false)
        {
            std::cout << (std::string)fu << "文件不存在" << std::endl;
            return false;
        }
        std::time_t curTime = std::time(nullptr);
        std::time_t lastWriteTime = fu.getLastWriteTime();
        if (curTime - lastWriteTime > hotTime_)
            return false;
        else
            return true;
    }
}