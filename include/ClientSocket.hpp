#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP
#include "Socket.hpp"

class ClientSocket : public Socket
{
private:
public:
    ClientSocket(int w_addr);
    ~ClientSocket();
};

#endif
