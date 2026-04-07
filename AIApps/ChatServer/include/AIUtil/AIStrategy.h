#pragma once
#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <sstream>
#include <memory>

#include "../../../../HttpServer/include/utils/JsonUtil.h"



class AIStrategy {
public:
    virtual ~AIStrategy() = default;// 定义接口

    
    virtual std::string getApiUrl() const = 0;// 获取API URL

    // API Key
    virtual std::string getApiKey() const = 0;// 获取API Key


    virtual std::string getModel() const = 0;// 获取模型名称


    virtual json buildRequest(const std::vector<std::pair<std::string, long long>>& messages) const = 0;// 构建请求体，参数是消息列表，返回值是JSON格式的请求体


    virtual std::string parseResponse(const json& response) const = 0;// 解析响应，参数是JSON格式的响应体，返回值是AI的回答字符串

    bool isMCPModel = false;

};

class AliyunStrategy : public AIStrategy {// 阿里云大模型的策略实现

public:
    AliyunStrategy() {
        const char* key = std::getenv("DASHSCOPE_API_KEY");// 从环境变量获取API Key，增强安全性，避免硬编码
        if (!key) throw std::runtime_error("Aliyun API Key not found!");// 如果环境变量中没有找到API Key，抛出异常
        apiKey_ = key;
        isMCPModel = false;
    }

    std::string getApiUrl() const override;
    std::string getApiKey() const override;
    std::string getModel() const override;

    json buildRequest(const std::vector<std::pair<std::string, long long>>& messages) const override;
    std::string parseResponse(const json& response) const override;

private:
    std::string apiKey_;
};

class DouBaoStrategy : public AIStrategy {

public:
    DouBaoStrategy() {
        const char* key = std::getenv("DOUBAO_API_KEY");
        if (!key) throw std::runtime_error("DOUBAO API Key not found!");
        apiKey_ = key;
        isMCPModel = false;
    }
    std::string getApiUrl() const override;
    std::string getApiKey() const override;
    std::string getModel() const override;

    json buildRequest(const std::vector<std::pair<std::string, long long>>& messages) const override;
    std::string parseResponse(const json& response) const override;

private:
    std::string apiKey_;
};

class AliyunRAGStrategy : public AIStrategy {

public:
    AliyunRAGStrategy() {
        const char* key = std::getenv("DASHSCOPE_API_KEY");
        if (!key) throw std::runtime_error("Aliyun API Key not found!");
        apiKey_ = key;
        isMCPModel = false;
    }

    std::string getApiUrl() const override;
    std::string getApiKey() const override;
    std::string getModel() const override;

    json buildRequest(const std::vector<std::pair<std::string, long long>>& messages) const override;
    std::string parseResponse(const json& response) const override;

private:
    std::string apiKey_;
};

class AliyunMcpStrategy : public AIStrategy {

public:
    AliyunMcpStrategy() {// 默认使用阿里云大模型的API Key，说明MCP模型也是基于阿里云大模型的
        const char* key = std::getenv("DASHSCOPE_API_KEY");
        if (!key) throw std::runtime_error("Aliyun API Key not found!");
        apiKey_ = key;
        isMCPModel = true;
    }

    std::string getApiUrl() const override;
    std::string getApiKey() const override;
    std::string getModel() const override;

    json buildRequest(const std::vector<std::pair<std::string, long long>>& messages) const override;
    std::string parseResponse(const json& response) const override;

private:
    std::string apiKey_;
};







