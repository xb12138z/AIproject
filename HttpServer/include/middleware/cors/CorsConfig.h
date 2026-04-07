#pragma once

#include <string>
#include <vector>

namespace http 
{
namespace middleware 
{

struct CorsConfig 
{
    std::vector<std::string> allowedOrigins;// 允许的源列表，支持通配符 "*"
    std::vector<std::string> allowedMethods;// 允许的HTTP方法列表
    std::vector<std::string> allowedHeaders;// 允许的请求头列表
    bool allowCredentials = false;// 是否允许携带凭证
    int maxAge = 3600;// 预检请求的缓存时间
    
    static CorsConfig defaultConfig() 
    {
        CorsConfig config;
        config.allowedOrigins = {"*"};
        config.allowedMethods = {"GET", "POST", "PUT", "DELETE", "OPTIONS"};
        config.allowedHeaders = {"Content-Type", "Authorization"};
        return config;
    }
};

} // namespace middleware
} // namespace http