#ifndef STREAM_SOCKET_HPP
#define STREAM_SOCKET_HPP

#include "Socket.hpp"

class StreamSocket : public Socket {
public:
    StreamSocket() : Socket(STREAM) {}
    StreamSocket(int fd) : Socket(fd, STREAM) {}
    virtual ~StreamSocket() {}
};

#endif
