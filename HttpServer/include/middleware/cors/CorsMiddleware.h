#pragma once

#include "../Middleware.h"
#include "../../http/HttpRequest.h"
#include "../../http/HttpResponse.h"
#include "CorsConfig.h"

namespace http 
{
namespace middleware 
{

class CorsMiddleware : public Middleware 
{
public:
    explicit CorsMiddleware(const CorsConfig& config = CorsConfig::defaultConfig());// 构造函数，接受CORS配置。浏览器为了安全，禁止前端网页直接访问不同域名的后端接口，CORS 就是用来允许这种跨域访问的机制。
    
    void before(HttpRequest& request) override;// 请求前处理，主要处理CORS预检请求（OPTIONS方法）
    void after(HttpResponse& response) override;// 响应后处理，添加CORS相关的响应头

    std::string join(const std::vector<std::string>& strings, const std::string& delimiter);

private:
    bool isOriginAllowed(const std::string& origin) const;
    void handlePreflightRequest(const HttpRequest& request, HttpResponse& response);
    void addCorsHeaders(HttpResponse& response, const std::string& origin);

private:
    CorsConfig config_;
};

} // namespace middleware
} // namespace http