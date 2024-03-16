#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <experimental/filesystem>
#include <jsoncpp/json/json.h>
#include "bundle.h"
#include "httplib.h"

namespace zwl
{
    namespace fileSystem = std::experimental::filesystem;
    class FileUtility
    {
    public:
        FileUtility(const std::string);              // 构造函数
        FileUtility(const FileUtility &);            // 拷贝构造
        FileUtility &operator=(const FileUtility &); // 赋值重载
        operator std::string();//类型转换函数
        fileSystem::file_type getFileType();         // 获取文件类型的函数
        std::string getFileName();                   // 获取文件名
        size_t getFileSize();                        // 获取文件大小
        std::string getLastWriteTimeStr();              // 获取最后一个修改时间
        std::time_t getLastWriteTime();              // 获取最后一个修改时间
        bool exists();                               // 判断是否文件存在
        bool create();
        bool createFile();                                 // 创建文件
        bool createDirectories();                          // 创建目录
        void scanDirectory(std::vector<std::string> *vec); // 获取目录项
        void remove();                                     // 移出文件（或目录）
        void setContentInFile(const std::string &content); // 设置内容进文件
        void getContentFromFile(std::string *content);     // 从文件中获取内容
        void compress(const std::string &packFilePath);    // 压缩文件
        void uncompress(const std::string &newFilePath);    // 解压文件
    private:
        fileSystem::path filePath_;
        fileSystem::file_type fileType_;
    };

    struct JsonUtility
    {
        static void serialize(const Json::Value &root, std::string *serializeStr);//序列化
        static void deserialize(const std::string &serializeStr, Json::Value *root);//反序列化
    };
}