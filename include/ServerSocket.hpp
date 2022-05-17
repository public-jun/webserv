#ifndef SERVER_SOCKET_HPP
#define SERVER_SOCKET_HPP

#include "Socket.hpp"
#include "SocketAddress.hpp"

#include <netinet/in.h>

class ServerSocket : public Socket
{
private:
    SocketAddress addr_;

public:
    static const unsigned short max_que = 1024;
    ServerSocket();
    ~ServerSocket();

    void bindSocket();

    void listenSocket();
};

#endif