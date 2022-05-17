#ifndef SOCKETADDRESS_HPP
#define SOCKETADDRESS_HPP

#include <netinet/in.h>
#include <string>

class SocketAddress
{
private:
    struct sockaddr_in socket_addr_;

public:
    static const unsigned short server_port = 4242;
    static const std::string    server_addr;
    SocketAddress();
    ~SocketAddress();

    const struct sockaddr_in& getSockaddr() const { return socket_addr_; }
};

#endif
