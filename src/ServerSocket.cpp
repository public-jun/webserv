#include <ServerSocket.hpp>

#include <iostream>

#include <cstdio>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

ServerSocket::ServerSocket()
{
    sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_ == -1)
    {
        std::cout << "socket error" << std::endl;
    }
    addr_ = SocketAddress();
}

ServerSocket::~ServerSocket() { close(sock_); }

void ServerSocket::bindSocket()
{
    if (bind(sock_, (const struct sockaddr*)&addr_.getSockaddr(),
             sizeof(addr_.getSockaddr())) == -1)
    {
        std::cout << "bind error" << std::endl;
        perror("bind");
        close(sock_);
        exit(1);
    }
}

void ServerSocket::listenSocket()
{
    if (listen(sock_, MAX_QUE) == -1)
    {
        std::cout << "listen error" << std::endl;
        perror("bind");
        close(sock_);
        exit(1);
    }
}
