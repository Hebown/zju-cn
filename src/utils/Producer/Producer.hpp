#include "MySocket/MySocket.hpp"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <glog/logging.h>

class Producer {
public:
    Producer(ClientSocket& socket, std::queue<std::vector<char>>& queue, std::mutex& mtx, std::condition_variable& cv)
        : socket_(socket), queue_(queue), mtx_(mtx), cv_(cv), running_(true) {}

    // 启动生产者线程
    void start() {
        thread_ = std::thread(&Producer::run, this);
    }

    // 停止生产者线程
    void stop() {
        running_ = false;
        if (thread_.joinable()) {
            thread_.join();
        }
    }

private:
    void run() {
        while (running_) {
            // 调用 ClientSocket::recv 接收数据
            ssize_t bytesReceived = socket_.recv(0);
            if (bytesReceived > 0) {
                // 获取接收到的数据
                std::vector<char> data = std::move(socket_.getSocketBuffer());
                {
                    std::unique_lock<std::mutex> lock(mtx_);
                    queue_.push(std::move(data)); // 写入共享队列
                    cv_.notify_all(); // 通知消费者
                }
                LOG(INFO) << "[Producer] Received " << bytesReceived << " bytes";
            } else if (bytesReceived == 0) {
                LOG(WARNING) << "[Producer] Server closed connection";
                break;
            } else {
                LOG(ERROR) << "[Producer] Receive failed: " << strerror(errno);
                break;
            }
        }
        running_ = false; 
        cv_.notify_all(); // 通知消费者线程退出
    }

    ClientSocket& socket_; // 引用 ClientSocket
    std::queue<std::vector<char>>& queue_; // 共享队列
    std::mutex& mtx_; // 共享互斥锁
    std::condition_variable& cv_; // 共享条件变量
    std::atomic<bool> running_; // 控制线程运行
    std::thread thread_; // 生产者线程
};