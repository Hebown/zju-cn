#pragma once
#include <cstddef>
#include <mutex>
#include <queue>
#include<sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
class MySocket{
    public:
        MySocket(int domain=AF_INET,int type=SOCK_STREAM,int protocol=0);
        ~MySocket(){close(socket_fd);}
        MySocket(const MySocket&)=delete;
        MySocket& operator=(const MySocket&)=delete;
        int get_socket_fd(){return socket_fd;}
    private:
        int socket_fd;
};


class ServerSocket:public MySocket{
    public:
        int bind(sockaddr* server_saddress,socklen_t server_saddr_len);
        int listen(int client_num);
        int accept(sockaddr* client_socket_addr,socklen_t* client_saddr_len);
        ssize_t send(int client_socket_addr,void*buffer,size_t buffer_size,int flags);
        void set_reuse_addr();
};

class ClientSocket:public MySocket{
    public:
        ClientSocket(size_t max_buffer_size=1024);
        int connect(sockaddr*server_socket_addr,socklen_t server_saddr_len);
        ssize_t recv(int flags=0);
        std::vector<char> getSocketBuffer();
    private:
        std::mutex mtx;
        size_t max_buffer_size;
        using  SocketBuffer = std::vector<char> ;
        std::queue<SocketBuffer>socket_buffer_queue;
};