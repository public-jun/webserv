#ifndef LISTENING_SOCKET_HPP
#define LISTENING_SOCKET_HPP

#include <map>
#include <string>

#include "Socket.hpp"

class ListeningSocket : public Socket {
public:
    ListeningSocket();
    virtual ~ListeningSocket();

    bool Bind(const std::string& ip, int port);
    bool Listen();
    void Close();

    // Add event
    bool Accept();

    // Do event
    void OnAccept();

private:
    static std::map<int, ListeningSocket*> all_listener;
};

#endif
