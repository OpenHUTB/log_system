#include "DataManage.hpp"
#include "Utility.hpp"
#include "CloudBackupConfig.hpp"

namespace zwl
{
    BackInfo::BackInfo(const std::string &backPath)
    {
        createBackInfo(backPath);
    }
    void BackInfo::createBackInfo(const std::string &backPath)
    {
        FileUtility fu(backPath);
        CloudBackupConfiger *configer = CloudBackupConfiger::getSingleton();
        int loopTime_ = 0;
        isPacked_ = false;
        backPath_ = backPath;
        packPath_ = configer->getPackDirPath() + fu.getFileName() + configer->getPackFileSuffix();
        url_ = configer->getDownloadPrefix() + fu.getFileName();
        fileSize_ = fu.getFileSize();
        // std::cout << (std::string)fu << " = " << fileSize_ << std::endl;
        lastWriteTime_ = fu.getLastWriteTime();
        lastWriteTimeStr_ = fu.getLastWriteTimeStr();
    }

    DataManager *DataManager::getSingleton()
    {
        static DataManager singleton;
        return &singleton;
    }
    DataManager::DataManager()
    {
        backupInfoFilePath_ = CloudBackupConfiger::getSingleton()->getBackupInfoFilePath();
        initLoad();
    }
    void DataManager::initLoad()
    {
        FileUtility fu(backupInfoFilePath_);
        if (fu.exists() == false)
        {
            fu.create();
        }
        std::string content;
        fu.getContentFromFile(&content);
        if (content == "")
        {
            dataManageTable_.clear();
            return;
        }
        Json::Value root;
        JsonUtility::deserialize(content, &root);
        for (const auto &item : root)
        {
            BackInfo temp;
            temp.isPacked_ = item["isPacked"].asInt();
            temp.backPath_ = item["backPath"].asString();
            temp.packPath_ = item["packPath"].asString();
            temp.url_ = item["url"].asString();
            temp.fileSize_ = item["fileSize"].asInt();
            temp.lastWriteTime_ = item["lastWriteTime"].asInt();
            temp.lastWriteTimeStr_ = item["lastWriteTimeStr"].asString();
            insert(temp);
        }
    }
    void DataManager::getBackInfoByUrlFromTable(const std::string &url, BackInfo *backInfo)
    {
        std::unique_lock<std::mutex> lck(tableMtx_);
        auto it = dataManageTable_.find(url);
        if (it == dataManageTable_.end())
        {
            std::cout << "未找到" << url << "对应的备份信息" << std::endl;
            return;
        }
        *backInfo = it->second;
    }
    void DataManager::getBackInfoByBackPathFromTable(const std::string &backPath, BackInfo *backInfo)
    {
        std::unordered_map<std::string, BackInfo> tempTable;
        {
            std::unique_lock<std::mutex> lck(tableMtx_);
            tempTable = dataManageTable_;
        }
        for (const auto &e : tempTable)
        {
            if (e.second.backPath_ == backPath)
            {
                *backInfo = e.second;
                return;
            }
        }
        std::cout << "未找到" << backPath << "对应的备份信息" << std::endl;
    }
    void DataManager::getAllBackInfoFromTable(std::vector<BackInfo> *vec)
    {
        std::unordered_map<std::string, BackInfo> tempTable;
        {
            std::unique_lock<std::mutex> lck(tableMtx_);
            tempTable = dataManageTable_;
        }
        for (const auto &e : tempTable)
            vec->push_back(e.second);
    }
    void DataManager::storage()
    {
        std::vector<BackInfo> vec;
        getAllBackInfoFromTable(&vec);
        Json::Value root;
        for (const auto &e : vec)
        {
            Json::Value item;
            item["isPacked"] = e.isPacked_;
            item["backPath"] = e.backPath_;
            item["packPath"] = e.packPath_;
            item["url"] = e.url_;
            item["fileSize"] = (Json::Int)e.fileSize_;
            item["lastWriteTime"] = (Json::Int)e.lastWriteTime_;
            item["lastWriteTimeStr"] = e.lastWriteTimeStr_;
            root.append(item);
        }
        std::string content;
        JsonUtility::serialize(root, &content);
        FileUtility fu(backupInfoFilePath_);
        fu.setContentInFile(content);
    }
    void DataManager::insert(const BackInfo &backinfo)
    {
        {
            std::unique_lock<std::mutex> lck(tableMtx_);
            dataManageTable_[backinfo.url_] = backinfo;
        }
        storage();
    }
    void DataManager::update(const BackInfo &backinfo)
    {
        insert(backinfo);
    }
}