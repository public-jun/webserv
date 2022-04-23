#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP
#include "Socket.hpp"

class ClientSocket : public Socket
{
private:
    struct sockaddr* addr_;

public:
    ClientSocket(int w_addr);
    ~ClientSocket();
};

#endif