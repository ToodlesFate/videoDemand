#ifndef __MY_DATA__
#define __MY_DATA__

#include "util.hpp"
#include <mysql/mysql.h>
#include <mutex>
#include <cstdlib>

namespace aod 
{
#define HOST "127.0.0.1"
#define USER "root"
#define PASS "123456"
#define NAME "aod_system"

    static MYSQL *MysqlInit() {
        MYSQL *mysql = mysql_init(nullptr);
        if (mysql == nullptr) {
            std::cout << "init mysql instance failed\n";
            return nullptr;
        }

        if (mysql_real_connect(mysql, HOST, USER, PASS, NAME, 0, nullptr, 0) == nullptr) {
            std::cout << "connect mysql server failed\n";
            return nullptr;
        }

        mysql_set_character_set(mysql, "utf8");
        return mysql;
    }

    static void MysqlDestory(MYSQL *mysql) {
        if (!mysql) {
            mysql_close(mysql);
        }
    }

    static bool MysqlQuery(MYSQL *mysql, const std::string &sql) {
        int ret = mysql_query(mysql, sql.c_str());
        if (ret != 0) {
            std::cout << sql << std::endl;
            std::cout << mysql_errno(mysql) << std::endl;
            return false;
        }
        return true;
    }

    class TableVideo
    {
    private:
        MYSQL *_mysql;
        std::mutex _mutex;
    public:
        TableVideo()
        {
            _mysql = MysqlInit();
            if (_mysql == nullptr) {
                exit(-1);
            }
        }

        ~TableVideo()
        {
            MysqlDestory(_mysql);
        }

        bool Insert(const Json::Value &video) {
            // id name info video image
            std::string sql;
            sql.resize(4096 + video["info"].asString().size());
            #define INSERT_VIDEO "insert tb_video values(null, '%s', '%s', '%s', '%s');"
            sprintf(&sql[0], INSERT_VIDEO, video["name"].asCString(), video["info"].asCString(), video["video"].asCString(), video["image"].asCString());
            return MysqlQuery(_mysql, sql);
        }

        bool Update(int video_id, const Json::Value &video) {
            std::string sql;
            sql.resize(4096 + video["info"].asString().size());      
            #define UPDATE_VIDEO "update tb_video set name = '%s', info = '%s' where id = %d;"
            sprintf(&sql[0], UPDATE_VIDEO, video["name"].asCString(), video["info"].asCString(), video_id);
            return MysqlQuery(_mysql, sql);    
        }

        bool Delete(int video_id) {
            #define DELETE_VIDEO "delete from tb_video where id = %d"
            char sql[1024] = {0};
            sprintf(sql, DELETE_VIDEO, video_id);
            return MysqlQuery(_mysql, sql);    
        }

        bool SelectAll(Json::Value *videos) {
            #define SELECT_VIDEO "select * from tb_video;"
            _mutex.lock();
            bool ret = MysqlQuery(_mysql, SELECT_VIDEO);
            if (ret == false) {
                _mutex.unlock();
                return false;
            }

            MYSQL_RES *res = mysql_store_result(_mysql);
            if (res == nullptr) {
                std::cout << "mysql store result failed!\n";
                _mutex.unlock();
                return false;
            }
            _mutex.unlock();

            int num_rows = mysql_num_rows(res);
            // id name info video image
            for (int i = 0; i < num_rows; ++ i) {
                MYSQL_ROW row = mysql_fetch_row(res);
                Json::Value video;
                video["id"] = row[0];
                video["name"] = row[1];
                video["info"] = row[2];
                video["video"] = row[3];
                video["image"] = row[4];
                videos->append(video);
            }
            mysql_free_result(res);

            return true;
        }

        bool SelectOne(int video_id, Json::Value *video) {
            #define SELECT_ONE_VIDEO "select * from tb_video where id = %d;"
            char sql[1024] = {0};
            sprintf(sql, SELECT_ONE_VIDEO, video_id);
            
            _mutex.lock();
            bool ret = MysqlQuery(_mysql, sql);
            if (ret == false) {
                _mutex.unlock();
                return false;
            }

            MYSQL_RES *res = mysql_store_result(_mysql);
            if (res == nullptr) {
                std::cout << "mysql store result failed!\n";
                _mutex.unlock();
                return false;
            }
            _mutex.unlock();

            int num_rows = mysql_num_rows(res);
            if (num_rows != 1) {
                std::cout << "have no data!\n";
                mysql_free_result(res);
                return false;
            }
        
            // id name info video image
            MYSQL_ROW row = mysql_fetch_row(res);
            (*video)["id"] = video_id;
            (*video)["name"] = row[1];
            (*video)["info"] = row[2];
            (*video)["video"] = row[3];
            (*video)["image"] = row[4];
            mysql_free_result(res);
            return true; 
        }

        bool SelectLike(const std::string &key, Json::Value *videos) {
            #define SELECT_Like_VIDEO "select * from tb_video where name like '%%%s%%';"
            char sql[1024] = {0};
            sprintf(sql, SELECT_Like_VIDEO, key.c_str());

            _mutex.lock();
            bool ret = MysqlQuery(_mysql, sql);
            if (ret == false) {
                std::cout << "---------------\n";
                _mutex.unlock();
                return false;
            }

            MYSQL_RES *res = mysql_store_result(_mysql);
            if (res == nullptr) {
                std::cout << "++++++++++++++++\n";
                std::cout << "mysql store result failed!\n";
                _mutex.unlock();
                return false;
            }
            _mutex.unlock();

            int num_rows = mysql_num_rows(res);
            std::cout << num_rows << std::endl;
            for (int i = 0; i < num_rows; ++ i) {
                MYSQL_ROW row = mysql_fetch_row(res);
                Json::Value video;
                video["id"] = row[0];
                video["name"] = row[1];
                video["info"] = row[2];
                video["video"] = row[3];
                video["image"] = row[4];
                videos->append(video);
            }
            mysql_free_result(res);
            return true;
        }
    };
};

#endif