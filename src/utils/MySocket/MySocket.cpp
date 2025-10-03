#include "MySocket.hpp"
#include <glog/logging.h>
#include <cerrno>
#include <mutex>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

MySocket::MySocket(int domain, int type, int protocol) {
    socket_fd = socket(domain, type, protocol);
    if (socket_fd < 0) {
        LOG(ERROR) << "Failed to create socket: " << strerror(errno);
        throw std::runtime_error("Failed to create socket: " + std::string(strerror(errno)));
    }
    LOG(INFO) << "Socket created successfully, fd: " << socket_fd;
}

int ServerSocket::bind(sockaddr* server_saddress, socklen_t server_saddr_len) {
    int ret = ::bind(get_socket_fd(), server_saddress, server_saddr_len);
    if (ret < 0) {
        LOG(ERROR) << "Bind failed: " << strerror(errno);
        throw std::runtime_error("Bind failed: " + std::string(strerror(errno)));
    }
    LOG(INFO) << "Socket bound successfully";
    return ret;
}

int ServerSocket::listen(int client_num) {
    int ret = ::listen(get_socket_fd(), client_num);
    if (ret < 0) {
        LOG(ERROR) << "Listen failed: " << strerror(errno);
        throw std::runtime_error("Listen failed: " + std::string(strerror(errno)));
    }
    LOG(INFO) << "Socket listening with backlog: " << client_num;
    return ret;
}

int ServerSocket::accept(sockaddr* client_socket_addr, socklen_t* client_saddr_len) {
    int client_fd = ::accept(get_socket_fd(), client_socket_addr, client_saddr_len);
    if (client_fd < 0) {
        LOG(ERROR) << "Accept failed: " << strerror(errno);
        throw std::runtime_error("Accept failed: " + std::string(strerror(errno)));
    }
    LOG(INFO) << "Accepted client connection, client fd: " << client_fd;
    return client_fd;
}

void ServerSocket::set_reuse_addr(){
    int opt = 1;
    if (setsockopt(get_socket_fd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        LOG(ERROR) << "Failed to set SO_REUSEADDR: " << strerror(errno);
        throw std::runtime_error("Failed to set SO_REUSEADDR: " + std::string(strerror(errno)));
    }
    LOG(INFO) << "SO_REUSEADDR set successfully";
}

ssize_t ServerSocket::send(int client_socket_addr, void* buffer, size_t buffer_size, int flags) {
    ssize_t sent = ::send(client_socket_addr, buffer, buffer_size, flags);
    if (sent < 0) {
        LOG(ERROR) << "Send failed: " << strerror(errno);
        throw std::runtime_error("Send failed: " + std::string(strerror(errno)));
    }
    LOG(INFO) << "Sent " << sent << " bytes to client fd: " << client_socket_addr;
    return sent;
}

ClientSocket::ClientSocket(size_t max_buffer_size) : max_buffer_size(max_buffer_size) {
    buffer=SocketBuffer(max_buffer_size);
    LOG(INFO) << "ClientSocket initialized with max buffer size: " << max_buffer_size;
}

int ClientSocket::connect(sockaddr* server_socket_addr, socklen_t server_saddr_len) {
    int ret = ::connect(get_socket_fd(), server_socket_addr, server_saddr_len);
    if (ret < 0) {
        LOG(ERROR) << "Connect failed: " << strerror(errno);
        throw std::runtime_error("Connect failed: " + std::string(strerror(errno)));
    }
    LOG(INFO) << "Connected to server successfully";
    return ret;
}

ssize_t ClientSocket::recv(int flags) {
    std::unique_lock<std::mutex>lock(mtx);
    ssize_t received = ::recv(get_socket_fd(), buffer.data(), max_buffer_size, flags);
    if (received < 0) {
        LOG(ERROR) << "Receive failed: " << strerror(errno);
        throw std::runtime_error("Receive failed: " + std::string(strerror(errno)));
    }else if(received==0){
        LOG(WARNING)<<"Received nothing!";
        buffer.clear();
    }else{
        buffer.resize(received);
        LOG(INFO) << "Received " << received << " bytes";
    }
    return received;
}

std::vector<char> ClientSocket::getSocketBuffer(){
    std::unique_lock<std::mutex>lock(mtx);
    return buffer;
}