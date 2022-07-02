#include "AcceptConn.hpp"

#include <netinet/in.h> // sockaddr_in

#include "EventRegister.hpp"
#include "ListeningSocket.hpp"
#include "RecvRequest.hpp"
#include "StreamSocket.hpp"
#include "SysError.hpp"

#include <iostream>

AcceptConn::AcceptConn(ListeningSocket listener)
    : IOEvent(listener.GetSocketFd(), ACCEPT_CONNECTION), listener_(listener),
      stream_sock_(StreamSocket(listener.GetServerConfig())) {}

AcceptConn::~AcceptConn() {}

void AcceptConn::Run(intptr_t offset) {
    UNUSED(offset);
    struct sockaddr_in peer_sin;
    int                len, stream_fd;

    len       = sizeof(peer_sin);
    stream_fd = accept(listener_.GetSocketFd(), (struct sockaddr*)&peer_sin,
                       (socklen_t*)&len);
    if (stream_fd < 0) {
        throw SysError("accept", errno);
    }

    stream_sock_.SetSocketFd(stream_fd);
    stream_sock_.SetAddress(peer_sin);

    printLog();
}

void AcceptConn::Register() { EventRegister::Instance().AddAcceptEvent(this); }

void AcceptConn::Unregister() { EventRegister::Instance().DelReadEvent(this); }

IOEvent* AcceptConn::RegisterNext() {
    IOEvent* recv_request = new RecvRequest(stream_sock_);

    recv_request->Register();
    return recv_request;
}

int AcceptConn::Close() { return 0; }

void AcceptConn::printLog() {
#ifdef WS_DEBUG
    std::cout << "=== AcceptConn ==="
              << "\n"
              << "listener fd: " << listener_.GetSocketFd() << "\n"
              << "stream fd  : " << stream_sock_.GetSocketFd() << "\n"
              << "=================="
              << "\n"
              << std::endl;
#endif
}
