#include "util.hpp"
#include "data.hpp"
#include "server.hpp"

void FileTest()
{
    aod::FileUtil("./www1").CreateDirectiory();
    aod::FileUtil("./www1/index.html").SetContent("<html></html>");
    std::string body;
    aod::FileUtil("./www1/index.html").GetContent(&body);
    std::cout << body << std::endl;
}

void JsonTest()
{
    Json::Value val;
    val["姓名"] = "小张";
    val["年龄"] = 18;
    val["性别"] = "男";
    val["成绩"].append(77.2);
    val["成绩"].append(72.2);
    val["成绩"].append(97.2);

    std::string body;
    aod::JsonUtil::Serialize(val, &body);
    std::cout << body << std::endl;

    Json::Value stu;
    aod::JsonUtil::UnSerialize(body, &stu);
    std::cout << stu["姓名"].asString() << std::endl;
    std::cout << stu["年龄"].asInt() << std::endl;
    std::cout << stu["性别"].asString() << std::endl;
    for (auto &a : stu["成绩"]) std::cout << a.asFloat() << std::endl;
}

void DataTest()
{
    aod::TableVideo tb_video;
    Json::Value video;
    // video["name"] = "变形金刚";
    // video["info"] = "aaaaaaaaaaaaaaaaaaaaa";
    // video["video"] = "/video/snake.ma4";
    // video["image"] = "/image/snake.jpg";
    // tb_video.Insert(video);
    // tb_video.Update(2, video);
    // tb_video.SelectAll(&video);
    // tb_video.SelectOne(1, &video);
    // tb_video.SelectLike("白娘子传", &video);
    // std::string body;
    // aod::JsonUtil::Serialize(video, &body);
    // std::cout << body << std::endl;

    tb_video.Delete(1);
}

void ServerTest()
{
    // aod::Server server(8080);
    aod::Server server(8080);
    server.RunModule();
}

int main()
{
    ServerTest();
    return 0;
}
