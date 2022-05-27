#ifndef LISTENING_SOCKET_HPP
#define LISTENING_SOCKET_HPP

#include <map>
#include <string>

#include "Socket.hpp"

class ListeningSocket : public Socket {
public:
    ListeningSocket();
    virtual ~ListeningSocket();

    void Bind(const std::string& ip, int port);
    void Listen();
    void Close();

    // Add event
    void Accept();

    // Do event
    void OnAccept();

private:
    static std::map<int, ListeningSocket*> all_listener;
};

#endif
