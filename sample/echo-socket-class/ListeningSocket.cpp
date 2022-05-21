#include "ListeningSocket.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>

#include <iostream>

ListeningSocket::ListeningSocket() {
    // type_ = Socket::LISTENING;
    SetSocketType(Socket::LISTENING);
}

ListeningSocket::~ListeningSocket() {}

bool ListeningSocket::Bind(const std::string& ip, int port) {
    sock_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd_ < 0) {
        // err
        std::cerr << "SOCKET ERR" << std::endl;
        return false;
    }
    all_listener_.insert(std::make_pair(sock_fd_, this));

    int sock_optval = 1;
    setsockopt(sock_fd_, SOL_SOCKET, SO_REUSEADDR, &sock_optval,
               sizeof(sock_optval));

    addr_.sin_family      = AF_INET;
    addr_.sin_port        = htons(port);
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());

    if (bind(sock_fd_, (struct sockaddr*)&addr_, sizeof(addr_)) < 0) {
        // err
        std::cerr << "BIND ERR" << std::endl;
        Close();
        return false;
    }
    return true;
}

bool ListeningSocket::Listen() {
    if (listen(sock_fd_, SOMAXCONN) < 0) {
        // err
        std::cerr << "BIND ERR" << std::endl;
        Close();
        return false;
    }

    // NonBlocking
    if (fcntl(sock_fd_, F_SETFL, O_NONBLOCK) < 0) {
        std::cerr << "BIND ERR" << std::endl;
        Close();
        return false;
    }
    return true;
}

void ListeningSocket::Close() {
    if (sock_fd_ != 0) {
        all_listener_.erase(sock_fd_);
        close(sock_fd_);
    }
}
