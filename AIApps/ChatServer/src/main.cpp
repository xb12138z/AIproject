#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <muduo/net/TcpServer.h>
#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>

#include"../include/ChatServer.h"

const std::string RABBITMQ_HOST = "localhost";
const std::string QUEUE_NAME = "sql_queue";
const int THREAD_NUM = 2;

void executeMysql(const std::string sql) {//给消息队列的回调函数，从消息队列中取出sql语句，执行mysql操作，用来消息的写入数据库
    http::MysqlUtil mysqlUtil_;
    mysqlUtil_.executeUpdate(sql);
}


int main(int argc, char* argv[]) {
	LOG_INFO << "pid = " << getpid();
	std::string serverName = "ChatServer";
	int port = 80;
    //进行端口的更新 
    int opt;
    const char* str = "p:";
    while ((opt = getopt(argc, argv, str)) != -1)//解析传入的argv参数，获取端口号，默认为80，如果传入了-p参数，则使用传入的端口号
    {
        switch (opt)
        {
        case 'p':
        {
            port = atoi(optarg);//将传入的端口号转换为整数，optarg是unistd.h中定义的全局变量，指向当前选项的参数字符串
            break;
        }
        default:
            break;
        }
    }

    muduo::Logger::setLogLevel(muduo::Logger::WARN);//设置muduo库的日志级别为WARN，减少日志输出，提升性能
    ChatServer server(port, serverName);//开启服务器，监听指定端口，等待客户端连接
    server.setThreadNum(4);//服务器使用4个线程处理客户端请求，提升并发性能
    
    std::this_thread::sleep_for(std::chrono::seconds(2));//等待2秒，确保服务器已经启动，避免在服务器未完全启动时就开始处理消息队列中的sql语句，导致mysql操作失败
    
    server.initChatMessage();//初始化聊天消息，从mysql数据库中读取聊天消息，存储在内存中，提升聊天消息的访问速度，减少mysql的访问次数


    RabbitMQThreadPool pool(RABBITMQ_HOST, QUEUE_NAME, THREAD_NUM, executeMysql);//创建一个RabbitMQThreadPool对象，连接到指定的RabbitMQ服务器和队列，使用指定数量的线程处理消息队列中的sql语句，executeMysql是回调函数，用来处理从消息队列中取出的sql语句
    pool.start();//启动线程池，开始处理消息队列中的sql语句

    server.start();//启动服务器，开始监听客户端连接，处理客户端请求
}
