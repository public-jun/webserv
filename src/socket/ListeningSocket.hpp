#ifndef LISTENING_SOCKET_HPP
#define LISTENING_SOCKET_HPP

#include <string>

#include "Socket.hpp"
class ListeningSocket : public Socket {
public:
    ListeningSocket();
    virtual ~ListeningSocket();

    void Bind(const std::string& ip, int port);
    void Listen();
    void Close();

private:
};

#endif
