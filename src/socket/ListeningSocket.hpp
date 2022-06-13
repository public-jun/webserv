#ifndef LISTENING_SOCKET_HPP
#define LISTENING_SOCKET_HPP

#include <string>
#include <vector>

#include "ServerConfig.hpp"
#include "Socket.hpp"
class ListeningSocket : public Socket {
public:
    ListeningSocket();
    ListeningSocket(std::vector<const ServerConfig> config);
    virtual ~ListeningSocket();

    const std::vector<const ServerConfig> GetServerConfig() const {
        return config_;
    }

    void Bind(const std::string& ip, int port);
    void Listen();
    void Close();

private:
    const std::vector<const ServerConfig> config_;
};

#endif
