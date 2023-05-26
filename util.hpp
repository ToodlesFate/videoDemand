#ifndef __MY_UTIL
#define __MY_UTIL

#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

#include <jsoncpp/json/json.h>
#include <sstream>
#include <memory>

#include <sys/types.h>
#include <sys/stat.h>

namespace aod
{
    class FileUtil
    {
    private:
        std::string _name; 
    public:
        FileUtil(const std::string name): _name(name)
        {}

        bool Exists() {
            int ret = access(_name.c_str(), F_OK);
            if (ret != 0) return false;
            return true;
        }

        size_t Size() {
            if (this->Exists() == false) {
                std::cout << "file not exist\n";
                return 0;
            }

            struct stat st;
            int ret = stat(_name.c_str(), &st);
            if (ret != 0) {
                std::cout << "get file stat failed \n";
                return 0;
            }
            return st.st_size;
        }

        bool GetContent(std::string *body) {
            std::ifstream ifs;
            ifs.open(_name, std::ios::binary);
            if (ifs.is_open() == false) {
                std::cout << "open failed \n";
                return false;
            }
            size_t flen = this->Size();
            body->resize(flen);
            ifs.read(&(*body)[0], flen);
            if (ifs.good() == false) {
                std::cout << "read file content failed\n";
                ifs.close();
                return false;
            }
            ifs.close();
            return true;
        }

        bool SetContent(const std::string& body) {
            std::ofstream ofs;
            ofs.open(_name, std::ios::binary);
            if (ofs.is_open() == false) {
                std::cout << "open failed \n";
                return false;
            }
            ofs.write(body.c_str(), body.size());
            if (ofs.good() == false) {
                std::cout << "write file content failed\n";
                ofs.close();
                return false;
            }
            ofs.close();
            return true;
        }

        bool CreateDirectiory() {
            if (this->Exists()) {
                return true;
            }
            mkdir(_name.c_str(), 0777);
            return true;
        }

    };

    class JsonUtil
    {
    public:
        static bool Serialize(const Json::Value &value, std::string *body) {
            Json::StreamWriterBuilder swb;
            std::unique_ptr<Json::StreamWriter> sw(swb.newStreamWriter());
            std::stringstream ss;
            int ret = sw->write(value, &ss);
            if (ret != 0) {
                std::cout << "Serialize failed!\n";
                return false;
            }
            *body = ss.str();
            return true;
        }

        static bool UnSerialize(const std::string &body, Json::Value *value) {
            Json::CharReaderBuilder crb;
            std::unique_ptr<Json::CharReader> cr(crb.newCharReader());

            std::string err;
            bool ret = cr->parse(body.c_str(), body.c_str() + body.size(), value, &err);
            if (ret == false) {
                std::cout << "UnSerialize failed" << std::endl;
                return false;
            }

            return true;
        }

    };
}

#endif