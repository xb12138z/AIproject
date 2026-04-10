#include"../include/AIUtil/MQManager.h"

// ------------------- MQManager -------------------
MQManager::MQManager(size_t poolSize)
    : poolSize_(poolSize), counter_(0) {
    for (size_t i = 0; i < poolSize_; ++i) {
        auto conn = std::make_shared<MQConn>();
        //  Create
        conn->channel = AmqpClient::Channel::Create("localhost", 5672, "guest", "guest", "/");

        pool_.push_back(conn);
    }
}

void MQManager::publish(const std::string& queue, const std::string& msg) {
    size_t index = counter_.fetch_add(1) % poolSize_;//轮询选择一个连接来发布消息，确保负载均衡
    auto& conn = pool_[index];//获取这个连接的锁，确保同一时间只有一个线程在使用这个连接发布消息，避免线程安全问题

    std::lock_guard<std::mutex> lock(conn->mtx);//构造一个 AMQP 消息对象，设置消息内容，然后通过这个连接的通道发布到指定的队列
    auto message = AmqpClient::BasicMessage::Create(msg);//创建一个 AMQP 消息对象，设置消息内容
    conn->channel->BasicPublish("", queue, message);//通过这个连接的通道发布到指定的队列
}

// ------------------- RabbitMQThreadPool -------------------

void RabbitMQThreadPool::start() {
    for (int i = 0; i < thread_num_; ++i) {
        workers_.emplace_back(&RabbitMQThreadPool::worker, this, i);
    }
}

void RabbitMQThreadPool::shutdown() {
    stop_ = true;
    for (auto& t : workers_) {
        if (t.joinable()) t.join();
    }
}

void RabbitMQThreadPool::worker(int id) {
    try {
        // Each thread has its own independent channel
        auto channel = AmqpClient::Channel::Create(rabbitmq_host_, 5672, "guest", "guest", "/");//当前线程创建自己专属的 RabbitMQ 通道
        // set exclusive
        channel->DeclareQueue(queue_name_, false, true, false, false);//确保这个队列存在
        // Prevent channel error: 403: AMQP_BASIC_CONSUME_METHOD caused: ACCESS_REFUSED - queue 
        // 'sql_queue' in vhost '/' in exclusive use
        // std::string consumer_tag = channel->BasicConsume(queue_name_, "");
        std::string consumer_tag = channel->BasicConsume(queue_name_, "", true, false, false);//告诉 RabbitMQ：这个线程现在开始订阅这个队列。返回一个 consumer_tag，后面可以用它继续收消息或取消订阅

        channel->BasicQos(consumer_tag, 1); //控制一次不要压太多未处理消息给这个消费者

        while (!stop_) {
            AmqpClient::Envelope::ptr_t env;
            bool ok = channel->BasicConsumeMessage(consumer_tag, env, 500); // 最多等待500ms ，尝试获取一条消息，如果没有消息就返回 false，继续循环等待下一次尝试
            if (ok && env) {
                std::string msg = env->Message()->Body();//把 RabbitMQ 里的消息内容拿出来
                handler_(msg);          //调用用户传入的处理函数，处理这条消息
                channel->BasicAck(env); //告诉 RabbitMQ 这条消息已经处理完了，可以从队列里删除了
            }
        }

        channel->BasicCancel(consumer_tag);//取消订阅
    }
    catch (const std::exception& e) {
        std::cerr << "Thread " << id << " exception: " << e.what() << std::endl;
    }
}
