#include "ClientSocket.hpp"
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

ClientSocket::ClientSocket(int server_sock)
{
    socklen_t addr_size = sizeof(addr_);
    sock_ = accept(server_sock, addr_, &addr_size);
    if (sock_ == -1)
    {
        std::cout << "accept error" << std::endl;
    }
}

ClientSocket::~ClientSocket() {}