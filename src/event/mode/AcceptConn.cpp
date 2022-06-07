#include "AcceptConn.hpp"

#include <netinet/in.h> // sockaddr_in

#include "EventAction.hpp"
#include "ListeningSocket.hpp"
#include "RecvRequest.hpp"
#include "StreamSocket.hpp"
#include "SysError.hpp"

#include <iostream>

AcceptConn::AcceptConn()
    : IOEvent(), listener_(ListeningSocket()), stream_sock_(StreamSocket()) {}

AcceptConn::AcceptConn(ListeningSocket listener)
    : IOEvent(listener.GetSocketFd(), ACCEPT_CONNECTION), listener_(listener),
      stream_sock_(StreamSocket()) {}

AcceptConn::~AcceptConn() {}

void AcceptConn::Run() {
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
}

IOEvent* AcceptConn::RegisterNext() {
    IOEvent* recv_request = new RecvRequest(stream_sock_);

    EventAction::GetInstance().AddReadEvent(recv_request);
    return recv_request;
}
