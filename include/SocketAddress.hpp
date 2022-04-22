#ifndef SOCKETADDRESS_HPP
#define SOCKETADDRESS_HPP

#include <netinet/in.h>
#include <string>

class SocketAddress
{
private:
    struct sockaddr_in socket_addr_;

public:
    static const unsigned short SERVER_PORT = 8080;
    static const std::string    SERVER_ADDR;
    SocketAddress();
    ~SocketAddress();

    const struct sockaddr_in& getSockaddr() const { return socket_addr_; }
};

#endif