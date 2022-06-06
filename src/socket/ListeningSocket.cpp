#include "ListeningSocket.hpp"

#include <arpa/inet.h> //inet_addr()
#include <cerrno>
#include <fcntl.h> // fcntl()
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // close()

#include "SysError.hpp"

ListeningSocket::ListeningSocket() { type_ = LISTENING; }

ListeningSocket::~ListeningSocket() {}

void ListeningSocket::Bind(const std::string& ip, int port) {
    sock_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd_ < 0) {
        throw SysError("socket", errno);
    }

    int sock_optval = 1;
    setsockopt(sock_fd_, SOL_SOCKET, SO_REUSEADDR, &sock_optval,
               sizeof(sock_optval));

    addr_.sin_family      = AF_INET;
    addr_.sin_port        = htons(port);
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());

    if (bind(sock_fd_, (struct sockaddr*)&addr_, sizeof(addr_)) < 0) {
        throw SysError("bind", errno);
    }
}

void ListeningSocket::Listen() {
    if (listen(sock_fd_, SOMAXCONN) < 0) {
        throw SysError("listen", errno);
    }

    if (fcntl(sock_fd_, F_SETFL, O_NONBLOCK) < 0) {
        throw SysError("fcntl", errno);
    }
}

void ListeningSocket::Close() {
    if (sock_fd_ == -1) {
        return;
    }

    if (close(sock_fd_) < 0) {
        throw SysError("close", errno);
    }
    sock_fd_ = -1;
}
