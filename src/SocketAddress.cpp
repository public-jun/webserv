#include "SocketAddress.hpp"

#include <arpa/inet.h>
#include <cstring>

const std::string SocketAddress::SERVER_ADDR = "127.0.0.1";

SocketAddress::SocketAddress() {
    std::memset(&socket_addr_, 0, sizeof(struct sockaddr_in));
    socket_addr_.sin_family = AF_INET;
    socket_addr_.sin_port   = htons(static_cast<unsigned short>(SERVER_PORT));
    socket_addr_.sin_addr.s_addr = inet_addr(SERVER_ADDR.c_str());
}

SocketAddress::~SocketAddress() {}
