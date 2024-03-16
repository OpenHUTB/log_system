#include "Server.hpp"
#include "CloudBackupConfig.hpp"
#include "DataManage.hpp"
#include "Utility.hpp"
#include "../LogAnalysis/Analysis.hpp"
namespace zwl
{
    Server::Server()
    {
        CloudBackupConfiger *configer = CloudBackupConfiger::getSingleton();
        serverIp_ = configer->getServerIp();
        serverPort_ = configer->getServerPort();
        downloadPrefix_ = configer->getDownloadPrefix();
    }
    Server *Server::getSingleton()
    {
        static Server singleton;
        return &singleton;
    }
    void Server::_ListShow(const httplib::Request &request, httplib::Response &response)
    {
        std::vector<BackInfo> vec;
        DataManager::getSingleton()->getAllBackInfoFromTable(&vec);
        std::stringstream ss;
        ss << "<html><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>文件列表</title><style>body{font-family:Arial,sans-serif;margin:20px;}h1{text-align:center;}table{width:100%;border-collapse:collapse;}th,td{padding:8px;text-align:left;}th{background-color:#f2f2f2;}tr:nth-child(even){background-color:#f9f9f9;}tr:hover{background-color:#f5f5f5;}a{text-decoration: none;}.refresh-btn {display: inline-block;padding: 10px 15px;background-color: #a7aaa7;color: white;text-decoration: none;border-radius: 4px;transition: background-color 0.3s;border: none;cursor: pointer;font-weight: bold;}.refresh-btn:hover {background-color: #0d0e0d;}</style></head>";
        ss << "<body><div><a href=\"#\"class=\"refresh-btn\"onclick=\"location.reload()\">刷新</a></div><table>";
        ss << "<tr><th>序号</th><th>文件名</th><th>最后修改时间</th><th>文件大小</th></tr>";
        int index = 1;
        for (const auto &e : vec)
        {
            ss << "<tr>";
            ss << "<td>" << index << ". </td>";
            ss << "<td><a href='" << e.url_ << "'>" << FileUtility(e.backPath_).getFileName() << "</a></td>";
            ss << "<td align='right'>" << e.lastWriteTimeStr_ << "</td>";
            ss << "<td align='right'>" << e.fileSize_ / 1024 << "kb</td>";
            ss << "</tr>";
            ++index;
        }
        ss << "</table></body></html>";
        response.body = ss.str();
        response.set_header("Content-Type", "text/html");
        response.status = 200;
    }
    std::string Server::getETag(const BackInfo &info)
    {

        FileUtility fu(info.backPath_);
        std::string etag = fu.getFileName() + "-" + std::to_string(fu.getFileSize()) + "-" + fu.getLastWriteTimeStr();
        return etag;
    }
    void Server::_Download(const httplib::Request &request, httplib::Response &response)
    {
        bool retrans = false;
        BackInfo info;
        DataManager::getSingleton()->getBackInfoByUrlFromTable(request.path, &info);
        FileUtility fu(info.packPath_);
        if (info.isPacked_ == true)
        {
            fu.uncompress(info.backPath_);
            info.isPacked_ = false;
            DataManager::getSingleton()->update(info);
        }
        std::string old_etag;
        if (request.has_header("If-Range") == true)
        // 没有这个字段则是正常下载，否则是断点续传，值是etag，如果返回的值与之前的值不一样，则必须重新下载
        {
            old_etag = request.get_header_value("If-Range");
            if (old_etag == getETag(info))
                retrans = true;
        }
        FileUtility fu1(info.backPath_);
        if (retrans == false)
        {

            fu1.getContentFromFile(&response.body);
            response.set_header("Accept-Ranges", "bytes");
            response.set_header("ETag", getETag(info));
            response.set_header("Content-Type", "application/octete-stream");
            response.status = 200;
            if (info.isPacked_ == false)
                fu.remove();
        }
        else
        {
            // string range = req.get_header_value("Range");bytes=start-end
            fu1.getContentFromFile(&response.body);
            response.set_header("Accept-Ranges", "bytes");
            response.set_header("ETag", getETag(info));
            response.set_header("Content-Type", "application/octete-stream");
            // rsp.set_header("Content-Range", "bytes start-end/file_size");
            response.status = 206;
        }
    }
    void _GetIpAndPort(const httplib::Request &request, httplib::Response &response)
    {
        std::string ip = request.remote_addr;
        int port = request.remote_port;

        // 构造响应内容
        std::string content = "Your IP address is: " + ip + "\nYour port number is: " + std::to_string(port);

        // 设置响应体
        response.set_header("Content-Type", "text/plain");
        response.set_header("Access-Control-Allow-Origin", "*");
        response.status = 200;
        response.set_content(content, "text/plain");
    }
    void Server::_Upload(const httplib::Request &request, httplib::Response &response)
    {
        auto ret = request.has_file("file");
        if (ret == false)
        {
            response.status = 400;
            return;
        }
        else
        {
            response.status = 200;
        }
        const httplib::MultipartFormData file = request.get_file_value("file");
        CloudBackupConfiger *configer = CloudBackupConfiger::getSingleton();
        std::string backPath = configer->getBackDirPath() + file.filename;
        FileUtility fu(backPath);
        fu.setContentInFile(file.content);
        DataManager::getSingleton()->insert(BackInfo(backPath));
        response.set_header("Access-Control-Allow-Origin", "*");
    }
    void Server::_Analysis(const httplib::Request &request, httplib::Response &response)
    {
        std::size_t found = request.path.find_last_of("/"); // 查找最后一个斜杠的位置
        std::string filename;
        if (found != std::string::npos)
        {                                              // 如果找到了斜杠
            filename = request.path.substr(found + 1); // 提取斜杠后面的子字符串
        }
        else
        {
            return;
        }
        std::string new_url = "/download/" + filename;
        // BackInfo info;
        // DataManager::getSingleton()->getBackInfoByUrlFromTable(new_url, &info);
        // if (info.isPacked_ == true)
        // {
        //     FileUtility(info.packPath_).uncompress(info.backPath_);
        //     info.isPacked_ = false;
        //     DataManager::getSingleton()->update(info);
        // }
        // FileUtility fu1(info.backPath_);
        //Analyzer::getSingleton()->analysis(info.backPath_);
        // std::thread([&]() -> void
        //             { Analyzer::getSingleton()->analysis(info.backPath_); })
        //     .detach();
        response.status = 200;
        response.set_header("Content-Type", "text/plain");
        response.set_header("Access-Control-Allow-Origin", "*");
        response.set_content(filename, "text/plain");
    }
    void Server::_Screen(const httplib::Request &request, httplib::Response &response)
    {
        std::size_t found = request.path.find_last_of("/"); // 查找最后一个斜杠的位置
        std::string filename;
        if (found != std::string::npos)
        {                                              // 如果找到了斜杠
            filename = request.path.substr(found + 1); // 提取斜杠后面的子字符串
        }
        else
        {
            return;
        }
        std::string new_url = "/download/" + filename;
        BackInfo info;
        DataManager::getSingleton()->getBackInfoByUrlFromTable(new_url, &info);
        if (info.isPacked_ == true)
        {
            FileUtility(info.packPath_).uncompress(info.backPath_);
            info.isPacked_ = false;
            DataManager::getSingleton()->update(info);
        }
        FileUtility fu1(info.backPath_);
        Analyzer::getSingleton()->analysis(info.backPath_);
        response.status = 200;
        response.set_header("Content-Type", "text/plain");
        response.set_header("Access-Control-Allow-Origin", "*");
    }
    void Server::runServer()
    {
        server_.Post("/upload", _Upload);
        server_.Get("/listshow", _ListShow);
        server_.Get("/", _ListShow);
        server_.Get("/getIpAndPort", _GetIpAndPort);
        std::string download_url = downloadPrefix_ + "(.*)";
        server_.Get(download_url, _Download);
        std::string analysisPrefix = "/analysis/";
        std::string analysis_url = analysisPrefix + "(.*)";
        server_.Get(analysis_url, _Analysis);
        std::string screenPrefix = "/screen/";
        std::string screen_url = screenPrefix + "(.*)";
        server_.Get(screen_url, _Screen);
        server_.listen(serverIp_, serverPort_);
    }
}