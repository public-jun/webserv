#ifndef STREAM_SOCKET_HPP
#define STREAM_SOCKET_HPP

#include "Socket.hpp"

class StreamSocket : public Socket
{
public:
    StreamSocket();
    virtual ~StreamSocket();

    int&     GetReadSize() { return read_size_; }
    ssize_t GetBufSize() { return buf_size_; }
    char*   GetBuf() { return buf_; }

    void Close();

private:
    int     read_size_;
    ssize_t buf_size_;
    char    buf_[2048];
};

#endif
