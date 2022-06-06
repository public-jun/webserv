#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <map>
#include <netinet/in.h>

class Socket {
public:
    enum SocketType {
        LISTENING,
        STREAM,
    };

    Socket() : sock_fd_(-1) {}
    Socket(int sock, SocketType type) : sock_fd_(sock), type_(type) {}
    Socket(SocketType type) : sock_fd_(-1), type_(type) {}
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
