#pragma once
#include <cstddef>
#include<sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
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
};

class ClientSocket:public MySocket{
    public:
    ClientSocket(void* buffer,size_t buffer_len);
    int connect(sockaddr*server_socket_addr,socklen_t server_saddr_len);
    ssize_t recv(int flags=0);
    private:
    
    void* buffer;
    size_t buffer_len;
};