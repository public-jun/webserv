#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <netinet/in.h>

class Socket
{
public:
    enum SocketType {
        LISTENING,
        STREAM,
    };

    Socket() : sock_fd_(0) {}
    Socket(int sock) : sock_fd_(sock) {}
    virtual ~Socket() {}

    void       SetSocketType(SocketType type) { type_ = type; }
    SocketType GetSocketType() { return type_; }

    void SetSocketFd(const int& sock) { sock_fd_ = sock; }
    int  GetSocketFd() { return sock_fd_; }

    void SetAddress(struct sockaddr_in addr) { addr_ = addr; }
    const struct sockaddr_in& GetAddr() { return addr_; }

protected:
    int                sock_fd_;
    SocketType         type_;
    struct sockaddr_in addr_;
};

#endif
