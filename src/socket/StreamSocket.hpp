#ifndef STREAM_SOCKET_HPP
#define STREAM_SOCKET_HPP

#include <vector>

#include "HTTPRequest.hpp"
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

    const HTTPRequest& GetRequest() const { return req_; }

    void SetRequest(HTTPRequest req) { req_ = req; }

private:
    const std::vector<const ServerConfig> config_;
    HTTPRequest                           req_;
};

#endif
