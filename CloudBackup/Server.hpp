#pragma once
#include <iostream>
#include <string>
#include "httplib.h"
#include "DataManage.hpp"

namespace zwl
{
    class Server
    {
    public:
        static Server *getSingleton();
        void runServer();

    private:
        // 加上using BackInfo = zwl::BackInfo;不然在getEtag函数里面会报错说 ‘BackInfo’ does not name a type
        using BackInfo = zwl::BackInfo;
        std::string serverIp_;
        int serverPort_;
        std::string downloadPrefix_;
        httplib::Server server_;
        
        Server();
        static void _ListShow(const httplib::Request &request, httplib::Response &response);
        static std::string getETag(const BackInfo &info);
        static void _Download(const httplib::Request &request, httplib::Response &response);
        static void _Upload(const httplib::Request &request, httplib::Response &response);
        static void _IpAndPort(const httplib::Request &request, httplib::Response &response);
        static void _Analysis(const httplib::Request &request, httplib::Response &response);
        static void _Screen(const httplib::Request &request, httplib::Response &response);
    };
}