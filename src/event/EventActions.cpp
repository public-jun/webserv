#include "EventActions.hpp"

#include <cerrno>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>

#include "ListeningSocket.hpp"
#include "Socket.hpp"
#include "StreamSocket.hpp"
#include "SysError.hpp"

#include <iostream>

EventActions::EventActions() { active_list_.resize(1024); }

EventActions::~EventActions() {}

void EventActions::Init() {
    kqueue_fd_ = kqueue();
    if (kqueue_fd_ < 0) {
        throw SysError("kqueue", errno);
    }
}

bool EventActions::AddAcceptEvent(Socket* sock) {
    if (sock) {
        if (sock->GetSocketType() != Socket::LISTENING) {
            return false;
        }

        struct kevent ev;
        EV_SET(&ev, sock->GetSocketFd(), EVFILT_READ,
               EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, (void*)sock);
        change_list_.push_back(ev);

        return true;
    }
    return false;
}

bool EventActions::AddRecvEvent(Socket* sock) {
    if (!sock) {
        return false;
    }

    struct kevent ev;
    EV_SET(&ev, sock->GetSocketFd(), EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR,
           0, 0, (void*)sock);
    change_list_.push_back(ev);
    return true;
}

bool EventActions::AddSendEvent(Socket* sock) {
    if (!sock) {
        return false;
    }

    struct kevent ev;
    EV_SET(&ev, sock->GetSocketFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0,
           (void*)sock);
    change_list_.push_back(ev);
    return true;
}

bool EventActions::DelEvent(Socket* sock) {
    if (!sock) {
        return false;
    }

    struct kevent read_ev;
    EV_SET(&read_ev, sock->GetSocketFd(), EVFILT_READ, EV_DELETE | EV_DISABLE,
           0, 0, NULL);
    change_list_.push_back(read_ev);

    struct kevent write_ev;
    EV_SET(&write_ev, sock->GetSocketFd(), EVFILT_WRITE, EV_DELETE | EV_DISABLE,
           0, 0, NULL);
    change_list_.push_back(write_ev);
    return true;
}

bool EventActions::DelRecv(Socket* sock) {
    if (!sock) {
        return false;
    }

    struct kevent read_ev;
    EV_SET(&read_ev, sock->GetSocketFd(), EVFILT_READ, EV_DELETE, 0, 0,
           (void*)sock);
    change_list_.push_back(read_ev);
    return true;
}

bool EventActions::DelSend(Socket* sock) {
    if (!sock) {
        return false;
    }

    struct kevent write_ev;
    EV_SET(&write_ev, sock->GetSocketFd(), EVFILT_WRITE, EV_DELETE | EV_DISABLE,
           0, 0, (void*)sock);
    change_list_.push_back(write_ev);
    return true;
}

void EventActions::ProcessEvent() {
    int event_size = 0;

    event_size =
        kevent(kqueue_fd_, &*change_list_.begin(), (int)change_list_.size(),
               &*active_list_.begin(), active_list_.size(), NULL);

    change_list_.clear();
    if (event_size < 0) {
        throw SysError("kevent", errno);
    }

    onEvent(active_list_, event_size);
}

void EventActions::onEvent(std::vector<struct kevent> active_list,
                           int                        event_size) {
    for (int i = 0; i < event_size; i++) {
        Socket* sock = reinterpret_cast<Socket*>(active_list[i].udata);
        if (!sock) {
            continue;
        }

        if (sock->GetSocketType() == Socket::LISTENING) {
            ListeningSocket* listener = dynamic_cast<ListeningSocket*>(sock);
            listener->OnAccept();
        } else if (sock->GetSocketType() == Socket::STREAM) {
            StreamSocket* s_sock = dynamic_cast<StreamSocket*>(sock);
            if (active_list[i].flags == EV_EOF) {
                s_sock->OnDisConnect();
                delete s_sock;
            } else {
                handleStreamSocketEvent(s_sock);
            }
        }
    }
}

void EventActions::handleStreamSocketEvent(StreamSocket* sock) {
    switch (sock->GetEventType()) {
    case StreamSocket::RECV_REQUEST:
        std::cout << "Recv REQUEST" << std::endl;
        sock->OnRecv();
        break;

    case StreamSocket::SEND_RESPONSE:
        std::cout << "Send RESPONSE" << std::endl;
        sock->OnSend();
        delete sock;
        break;

    default:
        break;
    }
}
