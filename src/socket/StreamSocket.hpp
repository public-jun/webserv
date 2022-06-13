#ifndef STREAM_SOCKET_HPP
#define STREAM_SOCKET_HPP

#include <vector>

#include "ServerConfig.hpp"
#include "Socket.hpp"

class StreamSocket : public Socket {
public:
    StreamSocket() : Socket(STREAM) {}
    StreamSocket(std::vector<const ServerConfig> config)
        : Socket(STREAM), config_(config) {}
    StreamSocket(int fd) : Socket(fd, STREAM) {}
    virtual ~StreamSocket() {}

    const std::vector<const ServerConfig> GetServerConfig() const {
        return config_;
    }

private:
    const std::vector<const ServerConfig> config_;
};

#endif
