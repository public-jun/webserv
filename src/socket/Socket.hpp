#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "EventActions.hpp"
#include <netinet/in.h>

class EventActions;
class Socket {
public:
    enum SOCKET_TYPE {
        LISTENING,
        STREAM,
    };

    Socket() : sock_fd_(0) { actions_ = NULL; }
    Socket(int sock) : sock_fd_(sock) {}
    virtual ~Socket() {}

    void        SetSocketType(SOCKET_TYPE type) { type_ = type; }
    SOCKET_TYPE GetSocketType() { return type_; }

    void SetSocketFd(const int& sock) { sock_fd_ = sock; }
    int  GetSocketFd() { return sock_fd_; }

    void SetAddress(struct sockaddr_in addr) { addr_ = addr; }
    const struct sockaddr_in& GetAddr() { return addr_; }

    void          SetEventActions(EventActions* actions) { actions_ = actions; }
    EventActions* GetEventActions() { return actions_; }

protected:
    int                sock_fd_;
    SOCKET_TYPE        type_;
    struct sockaddr_in addr_;

    EventActions* actions_;
};

#endif
