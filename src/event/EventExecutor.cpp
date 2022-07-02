#include "EventExecutor.hpp"

#include <iostream>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>

#include "EventRegister.hpp"
#include "HTTPResponse.hpp"
#include "HTTPStatus.hpp"
#include "IOEvent.hpp"
#include "SendError.hpp"
#include "StreamSocket.hpp"
#include "SysError.hpp"

EventExecutor::EventExecutor()
    : kqueue_fd_(-1),
      registered_list_(EventRegister::Instance().RegisteredEvents()) {
    active_list_.resize(1024);
}

EventExecutor::~EventExecutor() {}

void EventExecutor::Init() {
    kqueue_fd_ = kqueue();
    if (kqueue_fd_ < 0) {
        throw SysError("kqueue", errno);
    }
}

void EventExecutor::ShutDown() {
    if (kqueue_fd_ == -1) {
        return;
    }

    if (close(kqueue_fd_) < 0) {
        throw SysError("close", errno);
    }
    kqueue_fd_ = -1;
}

void EventExecutor::ProcessEvent() {
    int event_size = 0;

    event_size = kevent(kqueue_fd_, registered_list_.data(),
                        (int)registered_list_.size(), active_list_.data(),
                        active_list_.size(), NULL);

    registered_list_.clear();
    if (event_size < 0) {
        throw SysError("kevent", errno);
    }

    onEvent(active_list_, event_size);
}

void EventExecutor::onEvent(std::vector<struct kevent> event_vec,
                            int                        event_size) {
    for (int i = 0; i < event_size; i++) {
        IOEvent* event = reinterpret_cast<IOEvent*>(event_vec[i].udata);
        if (!event) {
            continue;
        }

        try {
            doEvent(event, event_vec[i].data);
            nextEvent(event);
        } catch (std::pair<StreamSocket, status::code> err) {
            errorNextEvent(event, new SendError(err.first, err.second));
        }
    }
}

void EventExecutor::doEvent(IOEvent* event, intptr_t offset) {
    event->Run(offset);
}

void EventExecutor::nextEvent(IOEvent* event) {
    IOEvent* next_event = event->RegisterNext();
    if (event->GetIOEventMode() != IOEvent::ACCEPT_CONNECTION &&
        next_event != event) {
        if (event->Close() == -1) {
            throw status::server_error;
        }
        delete event;
    }
}

void EventExecutor::errorNextEvent(IOEvent* prev, IOEvent* next) {
    prev->Unregister();
    if (prev->GetIOEventMode() != IOEvent::ACCEPT_CONNECTION) {
        prev->Close();
        delete prev;
    }
    nextEvent(next);
}
