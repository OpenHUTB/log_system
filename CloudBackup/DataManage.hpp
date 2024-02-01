#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <mutex>
#include <vector>
#include <unordered_map>
namespace zwl
{
    struct BackInfo
    {
        bool isPacked_;
        int loopTime_;
        std::string backPath_;
        std::string packPath_;
        std::string url_;
        int fileSize_;
        std::time_t lastWriteTime_;
        std::string lastWriteTimeStr_;
        BackInfo() = default;
        BackInfo(const std::string &backPath);
        void createBackInfo(const std::string &backPath);
    };
    class DataManager
    {
    public:
        static DataManager *getSingleton();
        void getBackInfoByUrlFromTable(const std::string &url, BackInfo *back_info);
        void getBackInfoByBackPathFromTable(const std::string &back_path, BackInfo *back_info);
        void getAllBackInfoFromTable(std::vector<BackInfo> *vec);
        void insert(const BackInfo &backInfo);
        void update(const BackInfo &backInfo);

    private:
        using UrlType = std::string;
        std::string backupInfoFilePath_;
        std::unordered_map<UrlType, BackInfo> dataManageTable_;
        std::mutex tableMtx_;
        DataManager();
        void initLoad();
        void storage();
    };
}