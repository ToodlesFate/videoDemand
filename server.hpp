#include "data.hpp"
#include "httplib.h"

namespace aod 
{
#define WWWROOT "./www"
#define VIDEO_ROOT "/video/"
#define IMAGE_ROOT "/image/"

    TableVideo *tb_video = nullptr;
    class Server {
        private:
            int _port;
            httplib::Server _svr;

        private:
            static void Insert(const httplib::Request &req, httplib::Response &rsp) {
                
                if(req.has_file("name") == false || req.has_file("info") == false || req.has_file("video") == false || req.has_file("image") == false) {
                    rsp.status = 400;
                    rsp.body = R"({"result":false, "reason":"上传的数据有错--"})";
                    rsp.set_header("Content-Type", "application/json");
                    return ;
                }

                /* 
                    MultipartFormData:
                        name: 字段名
                        content_type: 正文类型
                        filename: 文件名称
                        content: 内容的数据
                */ 
                httplib::MultipartFormData name = req.get_file_value("name");
                httplib::MultipartFormData info = req.get_file_value("info");
                httplib::MultipartFormData video = req.get_file_value("video");
                httplib::MultipartFormData image = req.get_file_value("image");

                std::string video_name = name.content;
                std::string video_info = info.content;

                // std::string video_path = WWWROOT;
                // video_path += VIDEO_ROOT;
                // video_path += video_name;
                // video_path += video.filename;

                std::string root = WWWROOT;
                std::string video_path = root + VIDEO_ROOT + video_name + video.filename;

                if (FileUtil(video_path).SetContent(video.content) == false) {
                    rsp.status = 500;
                    rsp.body = R"({"result":false, "reason":"视频文件不存在"})";
                    rsp.set_header("Content-Type", "application/json");
                    return ;
                }

                // std::string image_path = WWWROOT;
                // image_path += IMAGE_ROOT;
                // image_path += video_name;
                // image_path += image.filename;

                std::string image_path = root + IMAGE_ROOT + video_name + image.filename;
                if (FileUtil(image_path).SetContent(image.content) == false) {
                    rsp.status = 500;
                    rsp.body = R"({"result":false, "reason":"图片文件不存在"})";
                    rsp.set_header("Content-Type", "application/json");
                    return ;
                }

                Json::Value video_json;
                video_json["name"] = video_name;
                video_json["info"] = video_info;
                video_json["video"] = VIDEO_ROOT + video_name + video.filename;     
                video_json["image"] = IMAGE_ROOT + video_name + image.filename;     
                if (tb_video->Insert(video_json) == false) {
                    rsp.status = 500;
                    rsp.body = R"({"result":false, "reason":"数据库新增失败"})";
                    rsp.set_header("Content-Type", "application/json");
                    return ;      
                }
                rsp.set_redirect("/index.html", 303);
            }

            static void Update(const httplib::Request &req, httplib::Response &rsp) {
                // 1. 获取修改的视频信息
                // 1.1 视频id 
                // 1.2 修改后信息
                std::string id = req.matches[1];
                int video_id = atoi(id.c_str());
                Json::Value video;
                if (JsonUtil::UnSerialize(req.body, &video) == false) {
                    rsp.status = 400;
                    rsp.body = R"({"result":false, "reason":"新的视频信息格式失败"})";
                    rsp.set_header("Content-Type", "application/json");
                    return ;   
                }

                // 2. 修改数据库
                if (tb_video->Update(video_id, video) == false) {
                    rsp.status = 500;
                    rsp.body = R"({"result":false, "reason":"修改数据库新增失败"})";
                    rsp.set_header("Content-Type", "application/json");
                    return ;   
                }
                
            }

            static void Delete(const httplib::Request &req, httplib::Response &rsp) {
                // 1. 获取视频信息
                std::string id = req.matches[1];
                int video_id = atoi(id.c_str());
                // 2. 删除视频文件 及 图片
                Json::Value video;
                if (tb_video->SelectOne(video_id, &video) == false) {
                    rsp.status = 500;
                    rsp.body = R"({"result":false, "reason":"视频文件不存在"})";
                    rsp.set_header("Content-Type", "application/json");
                    return ;
                }
                
                std::string root = WWWROOT;
                std::string video_path = root + video["video"].asString();
                std::string image_path = root + video["image"].asString();
                remove(video_path.c_str());
                remove(image_path.c_str());

                // 3. 删除数据库信息
                if (tb_video->Delete(video_id) == false) {
                    rsp.status = 500;
                    rsp.body = R"({"result":false, "reason":"删除数据库信息失败"})";
                    rsp.set_header("Content-Type", "application/json");
                    return ;
                }
            }
    
            static void SelectOne(const httplib::Request &req, httplib::Response &rsp) {
                // 1. 获取视频 id 
                std::string id = req.matches[1];
                int video_id = atoi(id.c_str());

                // 2. 查询数据
                Json::Value video;
                if (tb_video->SelectOne(video_id, &video) == false) {
                    rsp.status = 500;
                    rsp.body = R"({"result":false, "reason":"查询数据库指定信息失败"})";
                    rsp.set_header("Content-Type", "application/json");
                    return ;
                }

                // 3. 组织影响正文
                JsonUtil::Serialize(video, &rsp.body);
                rsp.set_header("Content-Type", "application/json");
            }

            static void SelectAll(const httplib::Request &req, httplib::Response &rsp) {
                bool select_flag = true;
                std::string search_key;
                if (req.has_param("search") == true) {
                    select_flag = false;
                    search_key = req.get_param_value("search");
                }

                Json::Value videos;
                if (select_flag == true) {
                    if (tb_video->SelectAll(&videos) == false) {
                        rsp.status = 500;
                        rsp.body = R"({"result":false, "reason":"查询数据库所有信息失败"})";
                        rsp.set_header("Content-Type", "application/json");
                        return ;
                    } 
                } else {
                    if (tb_video->SelectLike(search_key, &videos) == false) {
                        rsp.status = 500;
                        rsp.body = R"({"result":false, "reason":"查询数据库匹配信息失败"})";
                        rsp.set_header("Content-Type", "application/json");
                        return ;
                    }
                }
                JsonUtil::Serialize(videos, &rsp.body);
                rsp.set_header("Content-Type", "application/json");
            }

        public:
            Server(int port) :_port(port) 
            {}

            bool RunModule() {
                // 1. 初始化操作 --- 初始化数据管理模块, 创建指定的目录 
                tb_video = new TableVideo();
                FileUtil(WWWROOT).CreateDirectiory();

                std::string root = WWWROOT;

                // std::string video_real_path = WWWROOT;
                // video_real_path += VIDEO_ROOT;
                std::string video_real_path = root + VIDEO_ROOT;
                FileUtil(video_real_path).CreateDirectiory();
                
                // std::string image_real_path = WWWROOT;
                // image_real_path += IMAGE_ROOT;

                std::string image_real_path = root + IMAGE_ROOT;
                FileUtil(image_real_path).CreateDirectiory();

                // 2. 搭建 http 服务器, 开始运行
                // 2.1 设置静态根目录
                _svr.set_mount_point("/", WWWROOT);
                // 2.2 添加处理与映射关系
                _svr.Post("/video", Insert);
                _svr.Delete("/video/(\\d+)", Delete);
                _svr.Put("/video/(\\d+)", Update);
                _svr.Get("/video/(\\d+)", SelectOne);
                _svr.Get("/video", SelectAll);
                // 2.3 启动服务器
                _svr.listen("0.0.0.0", _port);
                return true;
            }

    };
}
