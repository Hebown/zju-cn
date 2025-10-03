#include "MySocket/MySocket.hpp"
#include "glog.hpp"
#include <arpa/inet.h> 
#include <glog/logging.h>
#include <string>
#include <cstring> 
#include <vector>

// 定义服务端地址和端口
#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 5903

int main(int argc, char* argv[]) {
    // 初始化 glog
    auto glog=GlogWrapper(argv[0]);
    try {
        // 创建缓冲区
        std::vector<char>buffer(1024);

        // 创建 ClientSocket 实例
        ClientSocket client( sizeof(buffer) - 1);

        // 设置服务器地址信息
        struct sockaddr_in serverAddress;
        memset(&serverAddress, 0, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(SERVER_PORT);
        serverAddress.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);

        // 连接到服务器
        client.connect((struct sockaddr*)&serverAddress, sizeof(serverAddress));
        LOG(INFO) << "[Info] Connected to server at " << SERVER_ADDRESS << ":" << SERVER_PORT;

        // 接收消息 
        ssize_t bytesReceived = client.recv(0);
        buffer=client.getSocketBuffer();
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            LOG(INFO) << "[Info] Message from server:" << buffer.data();
        } else {
            LOG(WARNING) << "[Warning] No data received from server";
        }

    } catch (const std::exception& e) {
        LOG(ERROR) << "[Error] " << e.what();
        return 1;
    }

    return 0;
}