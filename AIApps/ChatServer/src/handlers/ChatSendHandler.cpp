#include "../include/handlers/ChatSendHandler.h"


void ChatSendHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp)
{
    try
    {

        auto session = server_->getSessionManager()->getSession(req, resp);//获取会话
        LOG_INFO << "session->getValue(\"isLoggedIn\") = " << session->getValue("isLoggedIn");//检查用户是否登录
        if (session->getValue("isLoggedIn") != "true")//如果没有登录，返回401 Unauthorized错误
        {

            json errorResp;
            errorResp["status"] = "error";
            errorResp["message"] = "Unauthorized";
            std::string errorBody = errorResp.dump(4);

            server_->packageResp(req.getVersion(), http::HttpResponse::k401Unauthorized,
                "Unauthorized", true, "application/json", errorBody.size(),
                errorBody, resp);
            return;
        }


        int userId = std::stoi(session->getValue("userId"));
        std::string username = session->getValue("username");

        std::string userQuestion;
        std::string modelType;
        std::string sessionId;

        auto body = req.getBody();
        if (!body.empty()) {
            auto j = json::parse(body);//解析请求体，获取用户输入的问题、模型类型和会话ID
            if (j.contains("question")) userQuestion = j["question"];//获取用户输入的问题
            if (j.contains("sessionId")) sessionId = j["sessionId"];//获取会话ID

            modelType = j.contains("modelType") ? j["modelType"].get<std::string>() : "1";//获取模型类型，默认为"1"
        }


        std::shared_ptr<AIHelper> AIHelperPtr;//获取用户对应的AIHelper实例，如果不存在则创建一个新的实例，并存储在服务器的chatInformation中，提升AIHelper实例的访问速度，减少重复创建实例的开销
        {
            std::lock_guard<std::mutex> lock(server_->mutexForChatInformation);

            auto& userSessions = server_->chatInformation[userId];

            if (userSessions.find(sessionId) == userSessions.end()) {

                userSessions.emplace( 
                    sessionId,
                    std::make_shared<AIHelper>()
                );
            }
            AIHelperPtr= userSessions[sessionId];
        }
        

        std::string aiInformation=AIHelperPtr->chat(userId, username,sessionId, userQuestion, modelType);//调用AIHelper的chat方法，获取AI的响应内容
        json successResp;
        successResp["success"] = true;
        successResp["Information"] = aiInformation;
        std::string successBody = successResp.dump(4);//封装成功响应，设置响应的状态行、头部和正文，返回给客户端

        resp->setStatusLine(req.getVersion(), http::HttpResponse::k200Ok, "OK");
        resp->setCloseConnection(false);
        resp->setContentType("application/json");
        resp->setContentLength(successBody.size());
        resp->setBody(successBody);
        return;
    }
    catch (const std::exception& e)
    {

        json failureResp;
        failureResp["status"] = "error";
        failureResp["message"] = e.what();
        std::string failureBody = failureResp.dump(4);
        resp->setStatusLine(req.getVersion(), http::HttpResponse::k400BadRequest, "Bad Request");
        resp->setCloseConnection(true);
        resp->setContentType("application/json");
        resp->setContentLength(failureBody.size());
        resp->setBody(failureBody);
    }
}









