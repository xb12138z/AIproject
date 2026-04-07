#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <regex>
#include <fstream>
#include <sstream>
#include <iostream>
#include "../../../../HttpServer/include/utils/JsonUtil.h"  


struct AITool {//工具的基本信息
    std::string name;
    std::unordered_map<std::string, std::string> params;//参数名和参数类型
    std::string desc;//工具的描述信息，方便AI理解工具的功能
};


struct AIToolCall {//工具调用的信息
    std::string toolName;
    json args;//工具调用的参数，使用json格式，方便扩展和解析
    bool isToolCall = false;//是否是工具调用，默认为false
};


class AIConfig {
public:
    bool loadFromFile(const std::string& path);
    std::string buildPrompt(const std::string& userInput) const;
    AIToolCall parseAIResponse(const std::string& response) const;
    std::string buildToolResultPrompt(const std::string& userInput,const std::string& toolName,const json& toolArgs,const json& toolResult) const;

private:
    std::string promptTemplate_;
    std::vector<AITool> tools_;

    std::string buildToolList() const;
};
