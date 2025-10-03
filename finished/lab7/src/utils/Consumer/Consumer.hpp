#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <glog/logging.h>
#include <atomic>

class Consumer {
public:
    Consumer(std::queue<std::vector<char>>& queue, std::mutex& mtx, std::condition_variable& cv)
        : queue_(queue), mtx_(mtx), cv_(cv), running_(true) {}

    // 启动消费者线程
    void start() {
        thread_ = std::thread(&Consumer::run, this);
    }

    // 停止消费者线程
    void stop() {
        running_ = false;
        cv_.notify_all(); // 唤醒等待中的消费者线程
        if (thread_.joinable()) {
            thread_.join();
        }
    }

private:
    void run() {
        while (running_) {
            std::vector<char> data;
            {
                std::unique_lock<std::mutex> lock(mtx_);
                // 等待队列非空或停止信号
                while (queue_.empty() && running_) {
                    cv_.wait(lock);
                }
                // 如果收到停止信号且队列为空，退出
                if (!running_ && queue_.empty()) {
                    break;
                }
                // 从队列取出数据
                if (!queue_.empty()) {
                    data = std::move(queue_.front());
                    queue_.pop();
                }
            }
            // 处理数据
            if (!data.empty()) {
                data.push_back('\0');
                LOG(INFO) << "[Consumer] Message from server: " << data.data();
            }
        }
        LOG(INFO) << "[Consumer] Stopped";
    }

    std::queue<std::vector<char>>& queue_; // 共享队列
    std::mutex& mtx_; // 共享互斥锁
    std::condition_variable& cv_; // 共享条件变量
    std::atomic<bool> running_; // 控制线程运行
    std::thread thread_; // 消费者线程
};