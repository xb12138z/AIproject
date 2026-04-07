#include"../include/AIUtil/AIConfig.h"

bool AIConfig::loadFromFile(const std::string& path) {//从文件加载配置，目前只有一套默认配置，后续可以扩展成多套配置
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[AIConfig] Unable to open configuration file: " << path << std::endl;
        return false;
    }

    json j;
    file >> j;

    // Parsing templates
    if (!j.contains("prompt_template") || !j["prompt_template"].is_string()) {//检查配置文件中是否包含prompt_template字段，并且该字段是否为字符串类型，如果不满足条件则输出错误信息并返回false
        std::cerr << "[AIConfig] prompt_template is missing" << std::endl;
        return false;
    }
    promptTemplate_ = j["prompt_template"].get<std::string>();

    // List of parsing tools
    if (j.contains("tools") && j["tools"].is_array()) {//检查配置文件中是否包含tools字段，并且该字段是否为数组类型，如果满足条件则解析工具列表，否则工具列表保持为空
        for (auto& tool : j["tools"]) {
            AITool t;
            t.name = tool.value("name", "");
            t.desc = tool.value("desc", "");
            if (tool.contains("params") && tool["params"].is_object()) {
                for (auto& [key, val] : tool["params"].items()) {
                    t.params[key] = val.get<std::string>();
                }
            }
            tools_.push_back(std::move(t));
        }
    }
    return true;
}

std::string AIConfig::buildToolList() const {//构建工具列表字符串，格式为：工具名(参数1, 参数2) ¡ú 工具描述信息
    std::ostringstream oss;
    for (const auto& t : tools_) {
        oss << t.name << "(";
        bool first = true;
        for (const auto& [key, val] : t.params) {
            if (!first) oss << ", ";
            oss << key;
            first = false;
        }
        oss << ") ¡ú " << t.desc << "\n";
    }
    return oss.str();
}

std::string AIConfig::buildPrompt(const std::string& userInput) const {//构建提示词字符串，将用户输入和工具列表填充到prompt_template中，返回最终的提示词字符串
    std::string result = promptTemplate_;
    result = std::regex_replace(result, std::regex("\\{user_input\\}"), userInput);
    result = std::regex_replace(result, std::regex("\\{tool_list\\}"), buildToolList());
    return result;
}

AIToolCall AIConfig::parseAIResponse(const std::string& response) const {//解析AI的响应，判断是否包含工具调用的信息，如果包含则提取工具名和参数，返回一个AIToolCall结构体，如果不包含则返回一个isToolCall为false的AIToolCall结构体
    AIToolCall result;
    try {
        // Try parsing as JSON
        json j = json::parse(response);

        if (j.contains("tool") && j["tool"].is_string()) {
            result.toolName = j["tool"].get<std::string>();
            if (j.contains("args") && j["args"].is_object()) {
                result.args = j["args"];
            }
            result.isToolCall = true;
        }
    }
    catch (...) {
        // Not JSON, directly return text response
        result.isToolCall = false;
    }
    return result;
}

std::string AIConfig::buildToolResultPrompt(//构建工具结果提示词字符串，将用户输入、工具调用信息和工具结果填充到一个新的提示词中，返回最终的提示词字符串
    const std::string& userInput,
    const std::string& toolName,
    const json& toolArgs,
    const json& toolResult) const
{
    std::ostringstream oss;
    oss << "下面是用户说的话：" << userInput << "\n"
        << "我刚才调用了工具 [" << toolName << "] ，参数为："
        << toolArgs.dump() << "\n"
        << "工具返回的结果如下：\n" << toolResult.dump(4) << "\n"
        << "请根据以上信息，用自然语言回答用户。";
    return oss.str();
}

