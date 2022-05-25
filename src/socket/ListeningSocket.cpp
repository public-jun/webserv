#include "ListeningSocket.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>

#include "StreamSocket.hpp"

#include <iostream>

std::map<int, ListeningSocket*> ListeningSocket::all_listener;

ListeningSocket::ListeningSocket() { SetSocketType(Socket::LISTENING); }

ListeningSocket::~ListeningSocket() {}

bool ListeningSocket::Bind(const std::string& ip, int port) {
    sock_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd_ < 0) {
        // err
        std::cerr << "SOCKET ERR" << std::endl;
        return false;
    }
    all_listener.insert(std::make_pair(sock_fd_, this));

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
    Accept();
    return true;
}

bool ListeningSocket::Accept() {
    if (actions_) {
        actions_->AddAcceptEvent(this);
    }
    return true;
}

void ListeningSocket::Close() {
    if (sock_fd_ != 0) {
        all_listener.erase(sock_fd_);
        close(sock_fd_);
    }
}

void ListeningSocket::OnAccept() {
    struct sockaddr_in peer_sin;
    int                len, new_sock;

    len      = sizeof(peer_sin);
    new_sock = accept(sock_fd_, (struct sockaddr*)&peer_sin, (socklen_t*)&len);

    StreamSocket* s_sock = new StreamSocket();
    s_sock->SetSocketFd(new_sock);
    s_sock->SetEventActions(actions_);
    s_sock->SetAddress(peer_sin);
    s_sock->Recv();
}
