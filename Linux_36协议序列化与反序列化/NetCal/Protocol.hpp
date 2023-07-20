#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <jsoncpp/json/json.h>
#include "Util.hpp"

// #define MYSELF 1

namespace protocol_ns
{
#define SEP " "
#define SEP_LEN strlen(SEP)
#define HEADER_SEP "\r\n"
#define HEADER_SEP_LEN strlen(HEADER_SEP)

    // "长度"\r\n""_x _op _y"\r\n
    // "10 + 20" => "7"\r\n""10 + 20"\r\n => 报头 + 有效载荷
    // 请求/响应 = 报头\r\n有效载荷\r\n

    std::string AddHeader(const std::string &str)
    {
        std::cout << "AddHeader 之前:\n"
                  << str << std::endl;
        std::string s = std::to_string(str.size());
        s += HEADER_SEP;
        s += str;
        s += HEADER_SEP;
        std::cout << "AddHeader 之后:\n"
                  << s << std::endl;
        return s;
    }

    // "7"\r\n""10 + 20"\r\n => "10 + 20"
    std::string RemoveHeader(const std::string &str, const int &len)
    {
        std::cout << "RemoveHeader 之前:\n"
                  << str << std::endl;

        std::string res = str.substr(str.size() - HEADER_SEP_LEN - len, len);

        std::cout << "RemoveHeader 之后:\n"
                  << res << std::endl;

        return res;
    }

    int ReadPackage(int sock, std::string &inbuffer, std::string *package)
    {
        std::cout << "ReadPackage inbuffer 之前:\n"
                  << inbuffer << std::endl;

        // 边读取
        char buffer[1024];
        ssize_t s = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (s <= 0)
            return -1; // 读取出错
        buffer[s] = 0;
        inbuffer += buffer;

        std::cout << "ReadPackage inbuffer 之中:\n"
                  << inbuffer << std::endl;

        // 边分析
        auto pos = inbuffer.find(HEADER_SEP);
        if (pos == std::string::npos)
            return 0;                                                    // 读取的不完善
        std::string lenStr = inbuffer.substr(0, pos);                    // 获取了头部字符串
        int len = Util::toInt(lenStr);                                   // "123" -> 123
        int targetpackageLen = lenStr.size() + len + 2 * HEADER_SEP_LEN; // 添加了报头的目标字符串长度
        if (inbuffer.size() < targetpackageLen)
            return 0;                                    // 读取的不完善
        *package = inbuffer.substr(0, targetpackageLen); // 提取报文
        // 到此为止，inbuffer 什么都没动
        inbuffer.erase(0, targetpackageLen); // 从inbuffer中直接移除整个报文

        std::cout << "ReadPackage inbuffer 之后:\n"
                  << inbuffer << std::endl;

        return len; // 读取成功
    }

    // Request && Response都要提供序列化和反序列化功能
    class Request
    {
    public:
        Request()
        {
        }

        Request(int x, int y, char op) : _x(x), _y(y), _op(op)
        {
        }

        // 目前 "_x _op _y"
        bool Serialize(std::string *outStr)
        {
            *outStr = "";
#ifdef MYSELF
            std::string x_string = std::to_string(_x);
            std::string y_string = std::to_string(_y);

            *outStr = x_string + SEP + _op + SEP + y_string;

            std::cout << "Request Serialize:\n"
                      << *outStr << std::endl;
#else
            Json::Value root; // Value：一种万能对象，接受任意的kv对象
            root["x"] = _x;
            root["y"] = _y;
            root["op"] = _op; // 填充字段

            // Json::FastWriter writer; // Writer：是用来进行序列化的 struct -> string
            Json::StyledWriter writer; // 将Json转成好看一点的字符串
            // {
            //     "op" : 43,
            //     "x" : 1,
            //     "y" : 1
            // }

            *outStr = writer.write(root);
#endif 
            return true;
        }

        bool Deserialize(const std::string &inStr)
        {
#ifdef MYSELF
            // inStr : 10 + 20 => [0]=>10, [1]=>+, [2]=>20
            // string -> vector
            std::vector<std::string> result;
            Util::StringSplit(inStr, SEP, &result);
            if (result.size() != 3)
                return false;
            if (result[1].size() != 1)
                return false;
            _x = Util::toInt(result[0]);
            _y = Util::toInt(result[2]);
            _op = result[1][0];
#else
            Json::Value root;
            Json::Reader reader; // Reader：用来进行反序列化的
            reader.parse(inStr, root);

            _x = root["x"].asInt();
            _y = root["y"].asInt();
            _op = root["op"].asInt();
#endif
            Print();
            return true;
        }

        void Print()
        {
            std::cout << "_x: " << _x << std::endl;
            std::cout << "_y: " << _y << std::endl;
            std::cout << "_op: " << _op << std::endl;
        }
        ~Request()
        {
        }

    public:
        // _x + _op + _y
        int _x;
        int _y;
        char _op;
    };

    class Response
    {
    public:
        Response()
        {
        }

        Response(int result, int code) : _result(result), _code(code)
        {
        }

        bool Serialize(std::string *outStr)
        {
            *outStr = "";
#ifdef MYSELF
            std::string res_string = std::to_string(_result);
            std::string code_string = std::to_string(_code);

            *outStr = res_string + SEP + code_string;

            std::cout << "Response Serialize:\n"
                      << *outStr << std::endl;
#else
            Json::Value root;
            root["result"] = _result;
            root["code"] = _code;

            // Json::FastWriter writer;
            Json::StyledWriter writer;
            *outStr = writer.write(root);
#endif
            return true;
        }

        bool Deserialize(const std::string &inStr)
        {
#ifdef MYSELF
            std::vector<std::string> result;
            Util::StringSplit(inStr, SEP, &result);
            if (result.size() != 2)
                return false;

            _result = Util::toInt(result[0]);
            _code = Util::toInt(result[1]);
#else
            Json::Value root;
            Json::Reader reader; // Reader：用来进行反序列化的
            reader.parse(inStr, root);

            _result = root["result"].asInt();
            _code = root["code"].asInt();
#endif
            Print();
            return true;
        }

        void Print()
        {
            std::cout << "_result: " << _result << std::endl;
            std::cout << "_code: " << _code << std::endl;
        }

        ~Response()
        {
        }

    public:
        int _result;
        int _code; // 0->success 1,2,3,4代表不同的错误
    };

}