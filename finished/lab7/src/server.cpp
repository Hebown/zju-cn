#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include <unistd.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include "MySocket/MySocket.hpp"
#include "glog.hpp"

#define SERVER_PORT 5903
#define MAX_CLIENT_QUEUE 1


void handleClient(int clientSocket){
    try {
        const std::string greetingMessage = "Hello from lyh!";
        ssize_t sent = send(clientSocket, greetingMessage.c_str(), greetingMessage.length(), 0);
        if (sent < 0) {
            LOG(ERROR) << "Failed to send message to client fd " << clientSocket << ": " << strerror(errno);
            throw std::runtime_error("Send failed");
        }
        LOG(INFO) << "Sent " << sent << " bytes to client fd: " << clientSocket;
    } catch (const std::exception& e) {
        LOG(ERROR) << "Error in handleClient (fd " << clientSocket << "): " << e.what();
    }
    close(clientSocket);
    LOG(INFO) << "Closed client connection (fd: " << clientSocket << ")";
}


int main(int argc, char* argv[]) {
    auto glog=GlogWrapper(argv[0]);
    
    try {
        // 创建 ServerSocket 实例
        ServerSocket server;

        // 设置 SO_REUSEADDR
        server.set_reuse_addr();

        // 设置服务器地址信息
        struct sockaddr_in serverAddress;
        memset(&serverAddress, 0, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        serverAddress.sin_port = htons(SERVER_PORT);

        // 绑定和监听
        server.bind((struct sockaddr*)&serverAddress, sizeof(serverAddress));
        server.listen(MAX_CLIENT_QUEUE);
        LOG(INFO) << "Server is listening on port " << SERVER_PORT << "...";

        std::vector<std::thread> threads;

        while (true) {
            try {
                // 接受客户端连接
                struct sockaddr_in clientAddress;
                socklen_t clientAddressLength = sizeof(clientAddress);
                int clientSocket = server.accept((struct sockaddr*)&clientAddress, &clientAddressLength);
                
                // 记录客户端信息
                char clientIP[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &clientAddress.sin_addr, clientIP, INET_ADDRSTRLEN);
                LOG(INFO) << "Accepted client connection from " << clientIP << ":" << ntohs(clientAddress.sin_port)
                          << ", fd: " << clientSocket;

                // 创建线程处理客户端
                threads.emplace_back(handleClient, clientSocket);
            } catch (const std::exception& e) {
                LOG(ERROR) << "Accept failed: " << e.what() << ". Continuing to listen...";
                continue; 
            }
        }
    } catch (const std::exception& e) {
        LOG(ERROR) << "Main error: " << e.what();
        return 1;
    }

    return 0;
}