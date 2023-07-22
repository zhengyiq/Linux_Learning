#include "HttpServer.hpp"
#include "Err.hpp"
#include "Util.hpp"
#include <unordered_map>
#include <memory>
#include <vector>

// std::string HandlerHttp(const std::string& request)
// {
//     // 确信request是一个完整的报文
//     // 给别人返回的是一个http response
//     std::cout << "-------------------------------------------------" << std::endl;
//     std::cout << request << std::endl;
//     return "";
// }
// GET / HTTP/1.1
// Host: 120.26.50.214:8899
// Connection: keep-alive
// Cache-Control: max-age=0
// Upgrade-Insecure-Requests: 1
// User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36 Edg/114.0.1823.82
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7
// Accept-Encoding: gzip, deflate
// Accept-Language: zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6

const std::string SEP = "\r\n";
// 一般一个webserver，不做特殊说明，如果用户默认访问'/'，我们决不能将整站给对方
// 需要添加默认首页，而且不能让用户访问wwwroot中的任何一个目录本身，也可以将每一个目录都带上一个默认首页
const std::string defaultHomePage = "index.html";
const std::string webRoot = "./wwwroot"; // web根目录
const std::string page_404 = "./wwwroot/err_404.html";

class HttpRequest
{
public:
    HttpRequest() : path_(webRoot)
    {
    }

    ~HttpRequest() {}

    void Print()
    {
        logMessage(Debug, "method: %s, url: %s, version: %s",
                   method_.c_str(), url_.c_str(), httpVersion_.c_str());
        // for(const auto & line : body_)
        //     logMessage(Debug, "-%s", line.c_str());
        logMessage(Debug, "path: %s", path_.c_str());
        logMessage(Debug, "suffix_: %s", suffix_.c_str());
    }

public:
    std::string method_;
    std::string url_;
    std::string httpVersion_;
    std::vector<std::string> body_;
    std::string path_;
    std::string suffix_;
};

HttpRequest Deserialize(std::string &message)
{
    HttpRequest req;
    std::string line = Util::ReadOneLine(message, SEP);
    Util::ParseRequestLine(line, &req.method_, &req.url_, &req.httpVersion_);
    std::cout << message << std::endl;
    while (!message.empty())
    {
        line = Util::ReadOneLine(message, SEP);
        req.body_.push_back(line);
    }

    // /a/b/c.html 不想让其从Linux的根目录开始访问，因此设置web根目录
    req.path_ += req.url_; // "webRoot/a/b/c.html"
    if (req.path_[req.path_.size()-1] == '/') 
        req.path_ += defaultHomePage; // 整个字符串最后一个字符
    
    auto pos = req.path_.rfind(".");
    if (pos == std::string::npos)
        req.suffix_ = ".html";
    else
        req.suffix_ = req.path_.substr(pos);
    
    return req;
}

std::string GetContentType(const std::string &suffix)
{
    std::string content_type = "Content-Type: ";
    if (suffix == ".html" || suffix == ".htm")
        content_type + "text/html";
    else if (suffix == ".css")
        content_type += "text/css";
    else if (suffix == ".js")
        content_type += "application/x-javascript";
    else if (suffix == ".png")
        content_type += "image/png";
    else if (suffix == ".jpg")
        content_type += "image/jpeg";
    else
    {
    }
    return content_type + SEP;
}

std::string HandlerHttp(std::string& message)
{
    // 1. 读取请求
    // 确信request是一个完整的报文
    // 给别人返回的是一个http response
    std::cout << "-------------------------------------------------" << std::endl;
    // std::cout << message << std::endl;
    // 资源，图片(.png, .jpg...)，网页(.html, .htm)，视频(.mp3)，音频(..)->文件！都要有自己的后缀！
    // 2. 反序列化和分析请求
    HttpRequest req = Deserialize(message);
    req.Print();
    // 3. 使用请求 
    std::string body;
    // /a/b/c.html 不想让其从Linux的根目录开始访问，因此设置web根目录
    std::string path = webRoot;
    path += req.url_; // "webRoot/a/b/c.html"
    std::string response;
    if (true == Util::ReadFile(req.path_, &body))
    {
        response = "HTTP/1.0 200 OK" + SEP;
        response += "Content-Length: " + std::to_string(body.size()) + SEP;
        // response += "Content-Type: text/html" + SEP;
        response += GetContentType(req.suffix_);
        response += SEP;
        response += body;
    }
    else
    {
        response = "HTTP/1.0 404 NotFound" + SEP;
        Util::ReadFile(page_404, &body);
        response += "Content-Length: " + std::to_string(body.size()) + SEP;
        response += GetContentType(req.suffix_);
        response += SEP;
        response += body;
    }

    // 4. 重定向测试
    // std::string response;
    // response = "HTTP/1.0 301 Moved Permanently" + SEP;
    // response += "Location: https://www.baidu.com/" + SEP;
    // response += SEP;

    // 5. cookie && session实验 // 识别出异常之后可以进行解决，可以避免用户信息泄露，但是不能完全避免安全问题
    // std::string response;
    // response = "HTTP/1.0 200 OK" + SEP;
    // response += "Set-Cookie: sessionid=1234abcd" + SEP;
    // response += SEP;

    return response;
}

static void usage(std::string proc)
{
    std::cout << "Usage:\n\t" << proc << " port\n" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }
    uint16_t port = atoi(argv[1]);
    std::unique_ptr<HttpServer> tsvr(new HttpServer(HandlerHttp, port)); // TODO
    tsvr->InitServer();
    tsvr->Start();

    return 0;
}