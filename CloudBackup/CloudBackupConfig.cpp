#include "CloudBackupConfig.hpp"
#include "Utility.hpp"
namespace zwl
{
    CloudBackupConfiger::CloudBackupConfiger()
    {
        readCloudBackupConfigFile();
    }
    CloudBackupConfiger *CloudBackupConfiger::getSingleton()
    {
        static CloudBackupConfiger singleton;
        return &singleton;
    }
    void CloudBackupConfiger::readCloudBackupConfigFile()
    {
        FileUtility configFileUtility(CloudBackupConfigFilePath);
        std::string buffer;
        configFileUtility.getContentFromFile(&buffer);
        Json::Value root;
        JsonUtility::deserialize(buffer, &root);
        hotTime_ = root["hotTime"].asInt();
        serverIp_ = root["serverIp"].asString();
        serverPort_ = root["serverPort"].asInt();
        downloadPrefix_ = root["downloadPrefix"].asString();
        packFileSuffix_ = root["packFileSuffix"].asString();
        backDirPath_ = root["backDirPath"].asString();
        packDirPath_ = root["packDirPath"].asString();
        backupInfoFilePath_ = root["backupInfoFilePath"].asString();
    }

}