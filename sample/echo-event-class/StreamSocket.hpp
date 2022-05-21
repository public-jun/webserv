#ifndef STREAM_SOCKET_HPP
#define STREAM_SOCKET_HPP

#include "Socket.hpp"

class StreamSocket : public Socket
{
public:
    StreamSocket();
    virtual ~StreamSocket();

    // Add event
    void Recv();
    void Send();
    void Close();

    // Do event
    void OnRecv();
    void OnSend();

private:
    int     read_size_;
    ssize_t buf_size_;
    char    buf_[2048];
};

#endif
