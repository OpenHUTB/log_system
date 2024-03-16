#include "Analysis.hpp"
#include "../CloudBackup/Utility.hpp"

const std::string host = "127.0.0.1";
const std::string user = "connector";
const std::string passwd = "123456";
const std::string db = "mydataset";
const unsigned int port = 3306;
namespace zwl
{
    Analyzer *Analyzer::getSingleton()
    {
        static Analyzer singleton;
        return &singleton;
    }
    Analyzer::Analyzer() : my(mysql_init(nullptr))
    {
    }
    Analyzer::~Analyzer()
    {
        mysql_close(my);
    }

    void Analyzer::startMysql()
    {
        if (my == nullptr)
        {
            std::cerr << "init error" << std::endl;
            return;
        }
        if (mysql_real_connect(my, host.c_str(), user.c_str(), passwd.c_str(), db.c_str(), port, 0, 0) == nullptr)
        {
            std::cerr << "connect mysql error" << std::endl;
            return;
        }
        std::cout << "connect mysql success" << std::endl;
    }
    void Analyzer::analysis(const std::string filePathName)
    {
        FileUtility fu(filePathName);
        std::string content;
        fu.getContentFromFile(&content);
        Json::Value root;
        JsonUtility::deserialize(content, &root);
        std::string query;
        for (const auto &item : root)
        {
            std::string label = item["label"].asString();
            std::string labelDesc = item["label_desc"].asString();
            std::string sentence = item["sentence"].asString();
            std::string keywords = item["keywords"].asString();
            // std::cout << "Label: " << label << std::endl;
            // std::cout << "Label Description: " << labelDesc << std::endl;
            // std::cout << "Sentence: " << sentence << std::endl;
            //std::cout << "Keywords: " << keywords << std::endl;
            // std::cout << std::endl;
            query = "create table if not exists `" + fu.getFileName() + "_label_tab` ( label int primary key, label_desc varchar(255), count int ,unique unique_data(label))";
            if (mysql_query(my, query.c_str()))
            {
                std::cerr << "Failed:" << query << std::endl;
                return;
            }
            else
            {
                //std::cout << "mysql success:" << query.c_str() << std::endl;
            }
            query = "insert into `" + fu.getFileName() + "_label_tab`(label,label_desc,count) values(" + label + ",'" + labelDesc + "',1) on duplicate key update count = count +1;";
            if (mysql_query(my, query.c_str()))
            {
                std::cerr << "Failed:" << query << std::endl;
                return;
            }
            else
            {
                //std::cout << "mysql success:" << query.c_str() << std::endl;
            }

            std::stringstream ss(keywords);
            std::string word;
            query = "create table if not exists `" + fu.getFileName() + "_keyword_tab` ( keyword varchar(255), count int ,unique unique_data(keyword))";
            if (mysql_query(my, query.c_str()))
            {
                std::cerr << "Failed:" << query << std::endl;
                return;
            }
            else
            {
                //std::cout << "mysql success:" << query.c_str() << std::endl;
            }
            while (getline(ss, word, ','))
            {
                query = "insert into `" + fu.getFileName() + "_keyword_tab`(keyword,count) values('" + word + "',1) on duplicate key update count=count+1";
                if (mysql_query(my, query.c_str()))
                {
                    std::cerr << "Failed:" << query << std::endl;
                    return;
                }
                else
                {
                    //std::cout << "mysql success:" << query.c_str() << std::endl;
                }
            }
        }


    }
}

// create table keyword_tab(
//     -> keyword varchar(255) primary key ,
//     -> count int ,
//     -> unique  unique_data(keyword));
 //select * from `001.json_label_tab` order by count desc,label asc limit 10 ;