#ifndef LISTENING_SOCKET_HPP
#define LISTENING_SOCKET_HPP

#include <map>
#include <string>

#include "Socket.hpp"

class ListeningSocket : public Socket
{
public:
    ListeningSocket(/* args */);
    virtual ~ListeningSocket();

    bool Bind(const std::string& ip, int port);
    bool Listen();
    // bool Accept();
    void Close();

private:
    static std::map<int, ListeningSocket*> all_listener_;
};

#endif
