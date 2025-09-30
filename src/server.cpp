// 引入一些必要的头文件
#include <iostream>
#include <string>
#include <cstring> // For memset
#include <thread>
#include <unistd.h> // For close
#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <vector>
#include "glog.hpp"
#define SERVER_PORT 5903
#define MAX_CLIENT_QUEUE 1

// int handleClient(int clientSocket){
    // // 创建socket
    // const std::string greetingMessage="Hello from server!";
    // int serverSocket=socket(AF_INET, SOCK_STREAM,0);
    // if(serverSocket<0){
    //     LOG(ERROR) << "Failed to create socket" << std::endl;
    //     return -1;
    // }
    // // 设置服务器地址信息
    // sockaddr_in serverAddress;
    // memset(&serverAddress,0,sizeof(serverAddress));
    // serverAddress.sin_family=AF_INET;
    // serverAddress.sin_addr.s_addr=INADDR_ANY;
    // serverAddress.sin_port=htons(SERVER_PORT);

    // //绑定socket到本地地址
    // if(bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress))<0){
    //     LOG(ERROR) << "Binding failed" << std::endl;
    //     return -1;
    // }

    // // 监听
    // if(listen(serverSocket, 1)<0){ // 一个线程干一个连接，这里指定listen还可以用于告诉OS这个socket是用于监听/接受连接的，不是用于发出请求的。
    //     LOG(ERROR)<<"Listening failed"<<std::endl;
    //     return -1;
    // }
    // LOG(INFO)<<"Listening in port "<<SERVER_PORT<<"..."<<std::endl;

    // // 连接
    // LOG(INFO)<<"Connected to Client!"<<std::endl;
// }
void handleClient(int clientSocket){
    const std::string greetingMessage="Hello from server!";
    send(clientSocket, greetingMessage.c_str(), greetingMessage.length(), 0);
    close(clientSocket);
}


int main(int argc,char*argv[]) {
    auto glog=GlogWrapper(argv[0]);
    
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    const char* greetingMessage = "Hello from server!";

    // 创建socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        LOG(ERROR) << "Failed to create socket" << std::endl;
        return -1;
    }

    // 设置服务器地址信息
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(SERVER_PORT);

    // 绑定socket到本地地址
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        LOG(ERROR) << "Binding failed" << std::endl;
        return -1;
    }

    // 开始监听客户端的连接请求
    if (listen(serverSocket, MAX_CLIENT_QUEUE) < 0) { // 最大连接数为20
        LOG(ERROR) << "Listening failed" << std::endl;
        return -1;
    }
    LOG(INFO) << "Server is listening on port " << SERVER_PORT << "...";
    
    std::vector<std::thread>threads;

    while (true) {
        // 接受请求
        clientSocket=accept(serverSocket, (sockaddr*)&clientAddress, &clientAddressLength);
        LOG_IF(ERROR, clientSocket < 0) << "A client failed to connect.";
        LOG_IF(INFO, clientSocket >= 0) << "A client has connected";
        if(clientSocket<0){
            return 0;
        }
        // 创建新线程
        threads.emplace_back(handleClient,clientSocket);
    }
    // // 接受客户端连接
    // clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
    // if (clientSocket < 0) {
    //     LOG(ERROR) << "Accepting connection failed" << std::endl;
    //     return -1;
    // }

    // LOG(INFO)<< "Connected to client!" << std::endl;
    // // 向客户端发送问好消息
    // send(clientSocket, greetingMessage, strlen(greetingMessage), 0);

    // // 关闭sockets
    // close(clientSocket);
    close(serverSocket);

    return 0;
}