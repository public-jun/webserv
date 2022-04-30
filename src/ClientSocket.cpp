#include "ClientSocket.hpp"
#include <iostream>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

ClientSocket::ClientSocket(int server_sock)
{
    sock_ = accept(server_sock, NULL, NULL);
    if (sock_ == -1)
    {
        std::cout << "accept error" << std::endl;
        throw std::runtime_error("accept error");
    }
}

ClientSocket::~ClientSocket() {}
