#pragma once
#include <iostream>
#include <string>
#include "mysql/mysql.h"

namespace zwl
{
    class Analyzer
    {
    private:
        MYSQL *my;
        Analyzer();

    public:
        ~Analyzer();
        static Analyzer *getSingleton();
        void startMysql();
        void analysis(const std::string);//参数为文件路径名
    };
}