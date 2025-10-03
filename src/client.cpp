#include "MySocket/MySocket.hpp"
#include "Producer/Producer.hpp"
#include "Consumer/Consumer.hpp"
#include "glog.hpp"
#include <arpa/inet.h>
#include <glog/logging.h>
#include <queue>
#include <mutex>
#include <condition_variable>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 5903

int main(int argc, char* argv[]) {
    auto glog = GlogWrapper(argv[0]);
    try {
        // 创建共享队列和同步工具
        std::queue<std::vector<char>> msgQueue;
        std::mutex mtx;
        std::condition_variable cv;

        // 创建 ClientSocket
        ClientSocket client(1024);
        struct sockaddr_in serverAddress;
        memset(&serverAddress, 0, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(SERVER_PORT);
        serverAddress.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);

        // 连接服务器
        client.connect((struct sockaddr*)&serverAddress, sizeof(serverAddress));
        LOG(INFO) << "[Info] Connected to server at " << SERVER_ADDRESS << ":" << SERVER_PORT;

        // 创建并启动生产者和消费者
        Producer producer(client, msgQueue, mtx, cv);
        Consumer consumer(msgQueue, mtx, cv);
        producer.start();
        consumer.start();

        // 临时等待（后续可添加消费者线程）
        std::this_thread::sleep_for(std::chrono::seconds(10));
        producer.stop();
        consumer.stop();

    } catch (const std::exception& e) {
        LOG(ERROR) << "[Error] " << e.what();
        return 1;
    }
    return 0;
}