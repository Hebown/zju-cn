#include "MySocket.hpp"
#include <glog/logging.h>
#include <cerrno>
#include <stdexcept>
#include <string>

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

ssize_t ServerSocket::send(int client_socket_addr, void* buffer, size_t buffer_size, int flags) {
    ssize_t sent = ::send(client_socket_addr, buffer, buffer_size, flags);
    if (sent < 0) {
        LOG(ERROR) << "Send failed: " << strerror(errno);
        throw std::runtime_error("Send failed: " + std::string(strerror(errno)));
    }
    LOG(INFO) << "Sent " << sent << " bytes to client fd: " << client_socket_addr;
    return sent;
}

ClientSocket::ClientSocket(void* buffer, size_t buffer_len) : buffer(buffer), buffer_len(buffer_len) {
    if (!buffer && buffer_len > 0) {
        LOG(ERROR) << "Invalid buffer: null pointer with non-zero length";
        throw std::invalid_argument("Invalid buffer: null pointer with non-zero length");
    }
    LOG(INFO) << "ClientSocket initialized with buffer size: " << buffer_len;
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
    if (!buffer || buffer_len == 0) {
        LOG(ERROR) << "Cannot receive: buffer is null or size is zero";
        throw std::runtime_error("Cannot receive: buffer is null or size is zero");
    }
    ssize_t received = ::recv(get_socket_fd(), buffer, buffer_len, flags);
    if (received < 0) {
        LOG(ERROR) << "Receive failed: " << strerror(errno);
        throw std::runtime_error("Receive failed: " + std::string(strerror(errno)));
    }
    LOG(INFO) << "Received " << received << " bytes";
    return received;
}