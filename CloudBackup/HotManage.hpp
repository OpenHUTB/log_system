#pragma once
#include <iostream>
#include <string>
namespace zwl
{
    class HotManager
    {
    public:
        static HotManager* getSingleton();
        void runHotManage();

    private:
        int hotTime_;
        std::string backDirPath_;
        std::string packDirPath_;
        std::string packFileSuffix_;
        HotManager();
        bool isHotFile(const std::string &backPath);
    };
}