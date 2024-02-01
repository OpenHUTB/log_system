#include "Utility.hpp"
#include <chrono>
#include <ctime>
#include <fstream>
// #include <jsoncpp/json/json.h>
#include "bundle.h"
namespace zwl
{
    FileUtility::FileUtility(const std::string filePath) : filePath_(filePath)
    {
        fileType_ = getFileType();
    }
    FileUtility::FileUtility(const FileUtility &fileUtility)
        : filePath_(fileUtility.filePath_), fileType_(fileUtility.fileType_)
    {
    }
    FileUtility &FileUtility::operator=(const FileUtility &fileUtility)
    {
        if (this != &fileUtility)
        {
            filePath_ = fileUtility.filePath_;
            fileType_ = fileUtility.fileType_;
        }
        return *this;
    }
    FileUtility::operator std::string()
    {
        return (std::string)filePath_;
    }
    fileSystem::file_type FileUtility::getFileType() // 获取文件类型的函数
    {
        return fileSystem::status(filePath_).type();
    }
    std::string FileUtility::getFileName() // 获取文件名
    {
        return filePath_.filename();
    }
    size_t FileUtility::getFileSize() // 获取文件大小
    {
        return fileSystem::file_size(filePath_);
    }
    std::string FileUtility::getLastWriteTimeStr() // 获取最后一个修改时间
    {
        const int bufferSize = 100;
        char buffer[bufferSize];
        std::time_t cftime = getLastWriteTime();
        std::strftime(buffer, bufferSize, "%Y-%m-%d %H:%M:%S", std::localtime(&cftime));
        return buffer;
    }

    std::time_t FileUtility::getLastWriteTime() // 获取最后一个修改时间
    {
        fileSystem::file_time_type ftime = fileSystem::last_write_time(filePath_);
        std::time_t cftime = std::chrono::system_clock::to_time_t(ftime);
        return cftime;
    }
    bool FileUtility::exists() // 判断是否文件存在
    {
        return fileSystem::exists(filePath_);
    }
    bool FileUtility::createFile() // 创建文件
    {
        std::ofstream ofs(filePath_);
        if (ofs.good())
        {
            fileType_ = fileSystem::file_type::regular;
            return true;
        }
        else
            return false;
    }
    bool FileUtility::createDirectories() // 创建目录
    {
        bool ret = fileSystem::create_directories(filePath_);
        if (ret)
        {
            fileType_ = fileSystem::file_type::directory;
            return true;
        }
        else
            return false;
    }
    bool FileUtility::create()
    {
        if (filePath_.extension() == "")
        {
            return createDirectories();
        }
        else
        {
            return createFile();
        }
    }
    void FileUtility::scanDirectory(std::vector<std::string> *vec) // 获取目录项
    {
        for (auto &e : fileSystem::directory_iterator(filePath_))
            vec->push_back(fileSystem::path(e).relative_path().string());
    }
    void FileUtility::remove()
    {
        fileSystem::remove_all(filePath_);
    }
    void FileUtility::setContentInFile(const std::string &content) // 设置内容进文件
    {
        if (exists() == false)
            createFile();
        std::ofstream ofs(filePath_);
        if (ofs.is_open() == false)
        {
            std::cout << filePath_ << "打开失败" << std::endl;
            return;
        }
        ofs.write(content.data(), content.size());
        if (ofs.good() == false)
        {
            std::cout << filePath_ << "可能写入内容失败" << std::endl;
            return;
        }
    }
    void FileUtility::getContentFromFile(std::string *content) // 从文件中获取内容
    {
        if (exists() == false)
        {
            std::cout << filePath_ << "不存在" << std::endl;
            return;
        }
        std::ifstream ifs(filePath_);
        if (ifs.is_open() == false)
        {
            std::cout << filePath_ << "打开失败" << std::endl;
            return;
        }
        content->resize(getFileSize());
        ifs.read(&(*content)[0], content->size());
        if (ifs.good() == false)
        {
            std::cout << filePath_ << "可能读取内容失败" << std::endl;
            return;
        }
    }
    void FileUtility::compress(const std::string &packFilePath) // 压缩文件
    {
        std::string str;
        getContentFromFile(&str);
        std::string packStr = bundle::pack(bundle::LZIP, str);
        FileUtility fu(packFilePath);
        fu.createFile();
        fu.setContentInFile(packStr);
    }
    void FileUtility::uncompress(const std::string &newFilePath) // 解压文件
    {
        std::string packStr;
        getContentFromFile(&packStr);
        std::string content = bundle::unpack(packStr);
        FileUtility fu(newFilePath);
        fu.createFile();
        fu.setContentInFile(content);
    }
    void JsonUtility::serialize(const Json::Value &root, std::string *serializeStr)
    {
        std::stringstream string_stream;
        Json::StreamWriterBuilder stream_writer_builder;
        std::unique_ptr<Json::StreamWriter> stream_writer(stream_writer_builder.newStreamWriter());
        stream_writer->write(root, &string_stream);
        *serializeStr = string_stream.str();
    }
    void JsonUtility::deserialize(const std::string &serializeStr, Json::Value *root)
    {
        Json::CharReaderBuilder char_reader_builder;
        std::unique_ptr<Json::CharReader> char_reader(char_reader_builder.newCharReader());
        char_reader->parse(serializeStr.data(), serializeStr.data() + serializeStr.size(), root, nullptr);
    }
}