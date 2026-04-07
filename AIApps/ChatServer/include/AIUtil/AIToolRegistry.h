#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <ctime>
#include <curl/curl.h>
#include "../../../../HttpServer/include/utils/JsonUtil.h"

class AIToolRegistry {
public:
    using ToolFunc = std::function<json(const json&)>;//工具函数类型，接受json参数，返回json结果

    AIToolRegistry();//构造函数，注册内置工具

    void registerTool(const std::string& name, ToolFunc func);//注册工具接口
    json invoke(const std::string& name, const json& args) const//调用工具接口
    bool hasTool(const std::string& name) const;//检查工具是否存在接口

private:
    std::unordered_map<std::string, ToolFunc> tools_;

    
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);//curl回调函数，写入响应数据
    static json getWeather(const json& args);//内置工具：获取天气
    static json getTime(const json& args);//内置工具：获取时间
};
