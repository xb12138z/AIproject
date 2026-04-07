#pragma once
#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <sstream>
#include <memory>
#include <functional>
#include <unordered_map>
#include <string>


#include"AIStrategy.h"

class StrategyFactory {

public:
    using Creator = std::function<std::shared_ptr<AIStrategy>()>;//定义一个函数类型 Creator，表示创建 AIStrategy 实例的函数

    static StrategyFactory& instance();//单例模式，获取工厂实例

    void registerStrategy(const std::string& name, Creator creator);//注册策略，参数是策略名称和创建函数

    std::shared_ptr<AIStrategy> create(const std::string& name);//创建策略实例，参数是策略名称，返回值是对应的 AIStrategy 实例，如果找不到则抛出异常

private:
    StrategyFactory() = default;
    std::unordered_map<std::string, Creator> creators;//存储策略名称和创建函数的映射
};





template<typename T>
struct StrategyRegister {//模板结构体，用于注册策略，参数是策略类 T，构造函数会调用工厂的 registerStrategy 方法注册策略
    StrategyRegister(const std::string& name) {
        StrategyFactory::instance().registerStrategy(name, [] {
            std::shared_ptr<AIStrategy> instance = std::make_shared<T>();
            return instance;
            });
    }
};

