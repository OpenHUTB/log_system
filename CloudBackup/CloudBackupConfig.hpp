#pragma once
#include <iostream>
#include <string>

namespace zwl
{
    // 相对于main.cpp的路径
    constexpr auto CloudBackupConfigFilePath = "./CloudBackup/CloudBackupConfigFile.json";

    class CloudBackupConfiger
    {
    public:
        static CloudBackupConfiger *getSingleton();
        inline int getHotTime()
        {
            return hotTime_;
        }
        inline std::string getServerIp()
        {
            return serverIp_;
        }
        inline int getServerPort()
        {
            return serverPort_;
        }
        inline std::string getDownloadPrefix()
        {
            return downloadPrefix_;
        }
        inline std::string getPackFileSuffix()
        {
            return packFileSuffix_;
        }
        inline std::string getBackDirPath()
        {
            return backDirPath_;
        }
        inline std::string getPackDirPath()
        {
            return packDirPath_;
        }
        inline std::string getBackupInfoFilePath()
        {
            return backupInfoFilePath_;
        }

    private:
        CloudBackupConfiger();
        void readCloudBackupConfigFile();
        int hotTime_;
        std::string serverIp_;
        int serverPort_;
        std::string downloadPrefix_;
        std::string packFileSuffix_;
        std::string backDirPath_;
        std::string packDirPath_;
        std::string backupInfoFilePath_;
    };
}